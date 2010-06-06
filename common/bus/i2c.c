// TWI ctrl
#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <util/twi.h>

#include "i2c.h"

int i2c_start_xfer(void) {
	if (i2c_mode == I2C_IDLE) {
		i2c_mode = I2C_BUSY;
		TWCR  = TWCR_START;
		return 0;
	}
	#if DEBUG_L(2)
	fprintf_P(stderr,PSTR("\n[i2c] start %d"),i2c_mode);
	#endif
	return -1;
}

int i2c_reset_xfer(void) {
	i2c_mode = I2C_BUSY;
	TWCR  = TWCR_RESET;
	return 0;
}

void twi_init(void) {
	fprintf_P(io_init,PSTR("\n[twi init..."));
	power_twi_enable();

	// Enable Pullups
	// FIXME: axon specific.
	DDRD&=(uint8_t)~((1<<0)|(1<<1));
	PORTD|=((1<<0)|(1<<1));

	// Disable TWI
	TWCR=0;
	//TWCR&=(uint8_t)~(1<<TWEN);

	// Set SCL Clock
	TWBR=TWI_BR_VAL;
	TWSR|=TWI_PS_MSK;

	// Set Slave ADDR
	//TWAR=I2C_SLAVE_ADDR<<1+I2C_GENERAL_CALL_EN;
	//TWAMR=I2C_SLAVE_ADDR_MSK<<1;

	// Enable TWI base settings
	i2c_mode = I2C_IDLE;
	TWCR = TWCR_BASE;
	fprintf_P(io_init,PSTR("done]"));
}

// Debug output for the TWI ISR
#define twi_printf(...)	fprintf(stdout,__VA_ARGS__);
#define twi_printf_P(...) fprintf_P(stdout,__VA_ARGS__);

inline static void twi_inter_on(void) { TWCR|=(1<<TWIE); }
inline static void twi_inter_off(void) { TWCR&=(uint8_t)~(1<<TWIE); }

ISR(TWI_vect) {
	uint8_t tw_status = (uint8_t)TW_STATUS;

	// Don't block more critical ISRs
	twi_inter_off();
	sei();

	switch(tw_status) {
	case TW_START:
	case TW_REP_START: {
		// Send Slave Addr.
		if	(w_data_buf_pos == 0 && w_data_buf_len != 0) {
			i2c_mode 	= I2C_MT;
			TWDR 		= dev_w_addr;
			TWCR 		= TWCR_BASE;

		}
		else if (r_data_buf_pos == 0 && r_data_buf_len != 0) {
			i2c_mode	= I2C_MR;
			TWDR		= dev_r_addr;
			TWCR		= TWCR_BASE;
		}
		else {
			twi_printf_P(PSTR("\n[err] {r,w}_data_buf_pos != 0\n"));
		}
	} break;
	// MASTER TRANSMIT
	case TW_MT_SLA_ACK: {
		// sla+w ack,
		// start writing data
		TWDR		= w_data_buf[0]; //[w_data_buf_pos]
		w_data_buf_pos	= 1;
		TWCR		= TWCR_BASE;
	} break;
	case TW_MT_DATA_ACK: {
		// data acked
		// send more data or rep_start to read.
		if (w_data_buf_pos == w_data_buf_len) {
			// Done writing data
			// issue repstart for read
			// FIXME: when read len = zero?
			i2c_mode = I2C_BUSY;
			TWCR = TWCR_START;
		}
		else {
			// More data to be writen.
			TWDR = w_data_buf[w_data_buf_pos];
			w_data_buf_pos++;
			TWCR = TWCR_BASE;
		}
	} break;

	// MASTER READ
	case TW_MR_SLA_ACK: {
		// sla+r ack
		// wait for first data packet.
		TWCR = TWCR_BASE;
	} break;

	case TW_MR_DATA_ACK: {
		// Data read, wait for next read with ack or nack

		r_data_buf[r_data_buf_pos] = TWDR;
		r_data_buf_pos ++;

		if (r_data_buf_pos == (r_data_buf_len-1) ) {
			// One more read to go, send nak
			TWCR = TWCR_NACK;
		}
		else if (r_data_buf_pos >= r_data_buf_len) {
			// No more data to read.
			i2c_mode = I2C_IDLE;
			// call the callback
			if (xfer_complete_cb != NULL)
				TWCR = xfer_complete_cb();
			else
				TWCR = TWCR_STOP;
		}
		else {
			// Continue to read data.
			TWCR = TWCR_BASE;
		}
	} break;

	case TW_MR_DATA_NACK: {
		// Done transmitting,
		// check packet length, call the cb
		r_data_buf[r_data_buf_pos] = TWDR;
		r_data_buf_pos ++;
		if (r_data_buf_pos != r_data_buf_len) {

			//FIXME: not enough data read, handle?
			#if DEBUG_L(1)
			twi_printf_P(PSTR("\n[err] i2c: short"));
			#endif
			r_data_buf_pos = 0;
			w_data_buf_pos = 0;
			TWCR = TWCR_START;

		}
		else {
			i2c_mode = I2C_IDLE;

			//call the callback
			if (xfer_complete_cb != NULL)
				TWCR = xfer_complete_cb();
			else
				TWCR = TWCR_STOP;
		}
	} break;
	// Errors
	case TW_MT_SLA_NACK: {
		// sla+w nack
		// restart bus and begin the same transmition again.
		w_data_buf_pos = 0;
		r_data_buf_pos = 0;
		i2c_mode = I2C_BUSY;
		TWCR = TWCR_START;
	} break;
	case TW_MT_DATA_NACK: {
		// data nacked, rep_start and retransmit.
		// FIXME: should reset or repstart?
		w_data_buf_pos = 0;
		r_data_buf_pos = 0;
		i2c_mode = I2C_BUSY;
		TWCR = TWCR_START;
	} break;
	case TW_MR_SLA_NACK: {
		// sla+r nack,
		//???? (rep_start/try again a few times)
		// reset entire transaction
		w_data_buf_pos = 0;
		r_data_buf_pos = 0;
		i2c_mode = I2C_BUSY;
		TWCR 	= TWCR_START;
		#if DEBUG_L(2)
		twi_printfP(PSTR("\n[err] i2c: SLA+R NACK"));
		#endif
	} break;

	// other
	case TW_BUS_ERROR: {
		#if DEBUG_L(1)
		twi_printf_P(PSTR("\n[err]TWI_BUS_ERROR\n"));
		#endif
		i2c_mode = I2C_IDLE; //XXX:??
		TWCR = TWCR_STOP;
	} break;
	case TW_MT_ARB_LOST:
	case TW_MR_ARB_LOST: {
		// Wait for stop condition.
		// Only needed for multi master bus.
		// Send Start when bus becomes free.
		w_data_buf_pos = 0;
		r_data_buf_pos = 0;
		i2c_mode = I2C_BUSY;
		TWCR = TWCR_START;
	} break;
	default: {
		twi_printf_P(PSTR("\n[i2c] unknown tw_status %x"),tw_status);
		TWCR |= (1<<TWINT);
	} break;
	}

	// Reenable twi interrupt.
	cli();
	twi_inter_on();
}

