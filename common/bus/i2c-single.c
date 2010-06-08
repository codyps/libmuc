// TWI ctrl
#include "defines.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <util/twi.h>

#include "i2c.h"
#include "i2c-single.h"

static struct i2c_trans *c_trans;
static uint8_t msg_idx; // current msg in c_trans->msgs[msg_idx].
static uint8_t buf_idx; // current pos in c_trans->msgs[msg_idx]->buf
static bool trans_complete;

void i2c_transfer(struct i2c_trans *trans)
{
	while(c_trans);

	c_trans = trans;
	TWCR = TWCR_START;
}

void i2c_main_handler(void)
{
	if(c_trans && trans_complete) {
		c_trans->cb(c_trans,/* status */ 0);
		c_trans = 0;
	}
}

void i2c_init_master(void) 
{
	fprintf_P(io_init,PSTR("\n[twi init..."));
	power_twi_enable();

	// Enable Pullups
	// FIXME: axon specific.
	DDRD &= (uint8_t) ~((1<<0)|(1<<1));
	PORTD |= ((1<<0)|(1<<1));

	// Disable TWI
	TWCR = 0;

	// Set SCL Clock
	TWBR = TWI_BR_VAL;
	TWSR = TWI_PS_MSK;

	// Set Slave ADDR
	//TWAR=I2C_SLAVE_ADDR<<1+I2C_GENERAL_CALL_EN;
	//TWAMR=I2C_SLAVE_ADDR_MSK<<1;

	// Enable TWI base settings
	TWCR = TWCR_BASE;
	fprintf_P(io_init,PSTR("done]"));
}

// Debug output for the TWI ISR
#define twi_printf(...)	fprintf(stdout,__VA_ARGS__);
#define twi_printf_P(...) fprintf_P(stdout,__VA_ARGS__);

inline static void twi_inter_on(void) { TWCR|=(1<<TWIE); }
inline static void twi_inter_off(void) { TWCR&=(uint8_t)~(1<<TWIE); }

ISR(TWI_vect)
{
	uint8_t tw_status = (uint8_t)TW_STATUS;
	uint8_t twcr = (uint8_t)TWCR;
	// Don't block more critical ISRs

	TWCR = (uint8_t)twcr & (uint8_t)~(1<<TWIE);
	sei();

	struct i2c_msg c_msg = c_trans->msgs[msg_idx];
	switch(tw_status) {
	case TW_START:
	case TW_REP_START: {
		// Send Slave Addr.
		uint8_t addr = c_msg->addr;
		TWDR = addr;
		twcr = TWCR_BASE;
	} break;
	// MASTER TRANSMIT
	case TW_MT_SLA_ACK: {
		// sla+w ack,
		// start writing data
		if (c_msg->len)
			TWDR    = c_msg->buf[0];
			buf_idx = 1;
			twcr    = TWCR_BASE;
		} else {
			//TODO: handle case where no data is to be transmitted.
		}
	} break;
	case TW_MT_DATA_ACK: {
		// data acked
		// send more data or rep_start to read.
	
		if (buf_idx < c_msg->len) {
			// more data to transmit.
			TWDR = c_msg->buf[buf_idx];
			buf_idx ++;
			twcr = TWCR_BASE;
		} else {
			// check if we have another message, if so repstart.
			if (msg_idx < c_trans->ct) {
				twcr = TWCR_START;
				buf_idx = 0;
				msg_idx ++;
			}
		}
	} break;

	// MASTER READ
	case TW_MR_SLA_ACK: {
		// sla+r ack
		// wait for first data packet.
		twcr = TWCR_BASE;
	} break;

	case TW_MR_DATA_ACK: {
		// Data read, wait for next read with ack or nack

		c_msg->buf[buf_idx] = TWDR;
		buf_idx++;

		if (buf_idx == (c_msg->len - 1)) {
			// One more read to go, send nak
			twcr = TWCR_NACK;
		} else if (buf_idx >= c_msg->len) {
			// No more data to read.
			// call the callback
			if (c_trans->cb != NULL)
				c_trans->cb(c_trans,0);

			twcr = TWCR_STOP;
		} else {
			// Continue to read data.
			twcr = TWCR_BASE;
		}
	} break;

	case TW_MR_DATA_NACK: {
		// Done transmitting,
		// check packet length, call the cb
		c_msg->buf[buf_idx] = TWDR;
		buf_idx ++;
		if (buf_idx != c_msg->len) {
			//FIXME: not enough data read, handle?
			#if DEBUG_L(1)
			twi_printf_P(PSTR("\n[err] i2c: short"));
			#endif
			r_data_buf_pos = 0;
			w_data_buf_pos = 0;
			twcr = TWCR_START;

		} else {
			//call the callback
			if (c_trans->cb != NULL)
				c_trans->cb(c_trans,0);
			
			twcr = TWCR_STOP;
		}
	} break;
	// Errors
	case TW_MT_SLA_NACK: {
		// sla+w nack
		// restart bus and begin the same transmition again.
		buf_idx = 0;
		msg_idx = 0;
		twcr = TWCR_START;
	} break;
	case TW_MT_DATA_NACK: {
		// data nacked, rep_start and retransmit.
		// FIXME: should reset or repstart?
		buf_idx = 0;
		twcr = TWCR_START;
	} break;
	case TW_MR_SLA_NACK: {
		// sla+r nack,
		//???? (rep_start/try again a few times)
		// reset entire transaction
		buf_idx = 0;
		msg_idx = 0;
		twcr 	= TWCR_START;
		#if DEBUG_L(2)
		twi_printfP(PSTR("\n[err] i2c: SLA+R NACK"));
		#endif
	} break;

	// other
	case TW_BUS_ERROR: {
		#if DEBUG_L(1)
		twi_printf_P(PSTR("\n[err]TWI_BUS_ERROR\n"));
		#endif
		twcr = TWCR_STOP;
	} break;
	//case TW_MT_ARB_LOST:
	case TW_MR_ARB_LOST: {
		// Wait for stop condition.
		// Only needed for multi master bus.
		// Send Start when bus becomes free.
		buf_idx = 0;
		msg_idx = 0;
		twcr = TWCR_START;
	} break;
	default: {
		twi_printf_P(PSTR("\n[i2c] unknown tw_status %x"),tw_status);
		twcr |= (1<<TWINT);
	} break;
	}

	// Reenable twi interrupt.
	cli();
	TWCR = twcr;
}

