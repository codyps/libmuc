// TWI ctrl
#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <util/twi.h>

#include "twi_i2c.h"
#include "i2c_HMC6343.h"

static tw_if_mode_t tw_if_mode = TW_MT;
static i2c_mode_t i2c_mode = I2C_STARTED;

int twi_start_xfer(void) {
	uint8_t tw_stat = TW_STATUS;
	if (i2c_mode == I2C_IDLE) {
		TWCR  = TWCR_START;
		i2c_mode = I2C_STARTED;
		return 0;
	}
	else {
		i2c_bad_mode_handler();
		return -1;
	}
}

void twi_init(void) {
	power_twi_enable();

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
	TWCR = TWCR_BASE;
}

ISR(TWI_vect) {
	uint8_t tw_stat = TW_STATUS;
// TWI BUS
	if	(tw_stat == TW_NO_INFO) {
		fprintf(stderr,PSTR("\n[error] TWI: no state information."));
	}
	else if (tw_stat == TW_BUS_ERROR) {
		fprintf(stderr,PSTR("\n[error] TWI: bus error"));
		i2c_mode = I2C_IDLE;
		tw_if_mode = TW_MT;
		TWCR = TWCR_STOP;
	}
	else if	((tw_stat == TW_START)|(tw_stat == TW_REP_START)) {
		// Send Slave Addr.
		if	(i2c_mode == I2C_STARTED) {
			TWDR 		= dev_w_addr;
			TWCR 		= TWCR_BASE;

			i2c_mode 	= I2C_WAIT_SLA_W_ACK;
			tw_if_mode 	= TW_MT;
		}
		else if (i2c_mode == I2C_SEND_SLA_R) {
			TWDR		= dev_r_addr;
			TWCR		= TWCR_BASE;

			i2c_mode	= I2C_WAIT_SLA_R_ACK;
			tw_if_mode	= TW_MR;
		}
		else i2c_bad_mode_handler();
	}

	// MASTER TRANSMIT
	else if (tw_stat == TW_MT_SLA_ACK) {
		// sla+w ack, 
		// start writing data
		if (i2c_mode == I2C_WAIT_SLA_W_ACK) {
			TWDR		= w_data_buf[0];
			TWCR		= TWCR_BASE;

			i2c_mode	= I2C_WAIT_DATA_W_ACK;
			w_data_buf_pos	= 1;
		}
		else i2c_bad_mode_handler();		
	}
	else if (tw_stat == TW_MT_SLA_NACK) {
		// sla+w nack
		// restart bus and begin the same transmition again.
		if (i2c_mode == I2C_WAIT_SLA_W_ACK) {
			i2c_mode 	= I2C_IDLE;
			tw_if_mode	= TW_MT;

			TWCR		= TWCR_RESET;
		}
		else i2c_bad_mode_handler();
	}
	else if (tw_stat == TW_MT_DATA_ACK) {
		// data acked
		// send more data or rep_start to read.
		if (i2c_mode == I2C_WAIT_DATA_W_ACK) {
			if (w_data_buf_pos == w_data_buf_len) {
				// Done writing data
				// issue repstart for read
				// FIXME: when read len = zero?
				TWCR		= TWCR_START;
				i2c_mode	= I2C_SEND_SLA_R;
			}
			else {
				// More data to be writen.
				TWDR = w_data_buf[w_data_buf_pos];
				w_data_buf_pos++;
				tw_if_mode = TW_MT;
				TWCR = TWCR_BASE;
			}
		}
		else i2c_bad_mode_handler();
	}
	else if (tw_stat == TW_MT_DATA_NACK) {
		// data nacked, rep_start and retransmit.
		if (i2c_mode == I2C_WAIT_DATA_W_ACK) {
			i2c_mode 	= I2C_IDLE;
			tw_if_mode	= TW_MT;
			// FIXME: should reset or repstart?
			TWCR		= TWCR_RESET;
		}
		else i2c_bad_mode_handler();
	}
	else if (tw_stat == TW_MT_ARB_LOST) {
		// Wait for stop condition.
		// Only needed for multi master bus.
		// Send Start when bus becomes free.
		i2c_mode 	= I2C_IDLE;
		tw_if_mode	= TW_MT;

		TWCR = TWCR_START;		
	}
// MASTER READ
	else if (tw_stat == TW_MR_SLA_ACK) {
		// sla+r ack
		// wait for first data packet.
		if (i2c_mode == I2C_WAIT_SLA_R_ACK) {
			i2c_mode = I2C_READ_DATA;
			TWCR 	 = TWCR_BASE;
		}
		else i2c_bad_mode_handler();
	}
	else if (tw_stat == TW_MR_SLA_NACK) {
		// sla+r nack, 
		//???? (rep_start/try again a few times)
		if (i2c_mode == I2C_WAIT_SLA_R_ACK) {
			//FIXME: continualy retrys sending read addr.			
			TWDR	= dev_r_addr;
			TWCR 	= TWCR_BASE;			
		}
		else i2c_bad_mode_handler();		
	}
	else if (tw_stat == TW_MR_DATA_ACK) {
		// Data read, wait for next read with ack or nack
		if (i2c_mode == I2C_READ_DATA) {
			r_data_buf[r_data_buf_pos] = TWDR;
			r_data_buf_pos ++;
			if (r_data_buf_pos == r_data_buf_len-1) {
				// One more read to go, send nak
				TWCR = TWCR_NACK;
				i2c_mode = I2C_READ_DATA_DONE;
			}
			else if (r_data_buf_pos >= r_data_buf_len) {
				// No more data to read.
				tw_if_mode	= TW_MT;
				i2c_mode	= I2C_IDLE;
				TWCR		= TWCR_STOP;
				// call the callback
				if (xfer_complete_cb != NULL)
					xfer_complete_cb();
			}
			else {
				// Continue to read data.
				TWCR	 = TWCR_BASE;
			}
		}
		else i2c_bad_mode_handler();
	}
	else if (tw_stat == TW_MR_DATA_NACK) {
		// Done transmitting, 
		// check packet length, call the cb
		if (i2c_mode == I2C_READ_DATA_DONE) {
			r_data_buf[r_data_buf_pos] = TWDR;
			r_data_buf_pos ++;
			if (r_data_buf_pos != r_data_buf_len) {
				//FIXME: not enough data read, handle?
				#if DEBUG_L(1)
				fprintf_P(stderr, PSTR("[error] i2c: data read shorter than expected at line %d"),__LINE__);
				#endif
			}
			i2c_mode	= I2C_IDLE;
			tw_if_mode	= TW_MT;
			TWCR		= TWCR_STOP;
		
			//call the callback
			if (xfer_complete_cb != NULL)
				xfer_complete_cb();
		}
		else i2c_bad_mode_handler();
	}
}

