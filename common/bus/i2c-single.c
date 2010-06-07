// TWI ctrl
#include "defines.h"

#include <stdio.h>

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

static volatile bool trans_complete;
static volatile uint8_t trans_status;

void i2c_transfer(struct i2c_trans *trans)
{
	while(c_trans);

	c_trans = trans;
	TWCR = TWCR_START;
}

void i2c_main_handler(void)
{
	if(c_trans && trans_complete) {
		c_trans->cb(c_trans, trans_status);
		c_trans = 0;
		trans_status = 0;
		trans_complete = 0;
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

#define NEXT_MSG() do {                \
	msg_idx++;                     \
	if (msg_idx < c_trans->ct) {   \
		twcr = TWCR_START;     \
	} else {                       \
		trans_complete = true; \
		twcr = TWCR_STOP;      \
	}                              \
} while(0)

#define TW_STOP(_status_) do {         \
	trans_status = _status_;       \
	trans_complete = true;         \
	twcr = TWCR_STOP;              \
} while (0)

static uint8_t exp_twsr;
ISR(TWI_vect)
{
	uint8_t tw_status = (uint8_t)TW_STATUS;

	/* clearing TWINT causes the bus to proceed, don't auto clear it */
	uint8_t twcr = (uint8_t)TWCR & (uint8_t)~(1<<TWINT);

	/* Don't block more critical ISRs */
	TWCR = (uint8_t)twcr & (uint8_t)~(1<<TWIE);
	sei();

	struct i2c_msg *c_msg = &(c_trans->msgs[msg_idx]);
	switch(tw_status) {
	case TW_START:
	case TW_REP_START:
		/* FIXME: assumes we sent the 'start' */
		TWDR = c_msg->addr;
		twcr = TWCR_BASE;
		break;

	/** MASTER TRANSMIT **/
	case TW_MT_SLA_ACK: {
		if (c_msg->len)
			TWDR    = c_msg->buf[0];
			buf_idx = 1;
			twcr    = TWCR_BASE;
		} else {
			/* No data to transmit, so we need to go to 
			 * the next msg */
			buf_idx = 0; /* should be 0 even without this */
			NEXT_MSG();	
		}
	} break;
	case TW_MT_DATA_ACK: {
		if (buf_idx < c_msg->len) {
			// more data to transmit.
			TWDR = c_msg->buf[buf_idx];
			buf_idx++;
			twcr = TWCR_BASE;
		} else {
			/* We are done with the present message, 
			 * move to the next */
			buf_idx = 0;
			NEXT_MSG();
		}
	} break;

	/** MASTER READ **/
	case TW_MR_SLA_ACK:
		/* wait for first data packet. */
		twcr = TWCR_BASE;
		break;

	case TW_MR_DATA_ACK: {
		/* Data read, wait for next read with ack or nack */

		c_msg->buf[buf_idx] = TWDR;
		buf_idx++;

		if (buf_idx == (c_msg->len - 1)) {
			/* One more read to go, send nak */
			twcr = TWCR_NACK;
		} else if (buf_idx >= c_msg->len) {
			/* No data left for this message, move to next */
			buf_idx = 0;
			NEXT_MSG();
		} else {
			/* Continue to read data. */
			twcr = TWCR_BASE;
		}
	} break;

	case TW_MR_DATA_NACK: {
		/* We nacked the last piece of data. */
		c_msg->buf[buf_idx] = TWDR;
		buf_idx ++;
		if (buf_idx != c_msg->len) {
			/* FIXME: not enough data read, handle? */
			#if DEBUG_L(1)
			twi_printf_P(PSTR("\n[err] i2c: short"));
			#endif
			TW_STOP(TW_MR_DATA_NACK);
		} else {
			/* looks like we expected this, and thus
			 * the message is done. move to the next
			 */
			buf_idx = 0;
			NEXT_MSG();
		}
	} break;
	
	/* NACKs : we don't know how to recover from these,
	 * let the callback deal with it. */
	case TW_MT_SLA_NACK: 
	case TW_MT_DATA_NACK:
	case TW_MR_SLA_NACK:
		TW_STOP(tw_status);
		break;
	
	case TW_BUS_ERROR: {
		#if DEBUG_L(1)
		twi_printf_P(PSTR("\n[i2c] TWI_BUS_ERROR\n"));
		#endif
		TW_STOP(TW_BUS_ERROR);
	} break;

	/* case TW_MT_ARB_LOST: */
	case TW_MR_ARB_LOST: {
		/* Send Start when bus becomes free. */
		buf_idx = 0;
		msg_idx = 0;
		twcr = TWCR_START;
	} break;

	default: {
		twi_printf_P(PSTR("\n[i2c] unk twsr %x"),tw_status);
		twcr |= (1<<TWINT);
	} break;
	}

	cli();
	/* Unstick the TWI hw */
	TWCR = twcr;
}

