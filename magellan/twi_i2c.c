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

/** Debug **/
#if DEBUG_L(1)
PGM_P i2c_mode_error_str = "\n[error] i2c: invalid mode 0x%x and %d on line %d";
#define i2c_error_invalid_mode() fprintf(stderr, i2c_mode_error_str ,tw_stat , i2c_mode, __LINE__); 
#else 
#define i2c_error_invalid_mode()
#endif // DEBUG_L(1)

static tw_if_mode_t tw_if_mode = TW_MT;
static i2c_mode_t i2c_mode = I2C_IDLE;

int i2c_start_xfer(void) {
	uint8_t tw_stat = TW_STATUS;
	if (i2c_mode == I2C_IDLE) {
		printf_P(PSTR("\n  Valid mode"));
		i2c_mode = I2C_STARTED;
		TWCR  = TWCR_START;
		return 0;
	}
	else {
		i2c_error_invalid_mode();
		return -1;
	}
}

void twi_init(void) {
	power_twi_enable();
	fprintf_P(stderr,PSTR("\n[twi] init:\t"));
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
	fprintf_P(stderr,PSTR("done"));
}

ISR(TWI_vect) {
	uint8_t tw_stat = TW_STATUS;
	// TWI BUS
	fprintf(stderr,PSTR("\n[debug] TWI ISR"));
	if	(tw_stat == TW_NO_INFO) {
		fprintf(stderr,PSTR("\n[error] TWI: no state information."));
	}
	else if (tw_stat == TW_BUS_ERROR) {
		fprintf(stderr,PSTR("\n[error] TWI: bus error."));
		i2c_mode = I2C_IDLE;
		tw_if_mode = TW_MT;
		TWCR = TWCR_STOP;
	}
	else if	((tw_stat == TW_START)|(tw_stat == TW_REP_START)) {
		// Send Slave Addr.
		if	(w_data_buf_pos == 0 && w_data_buf_len != 0) {
			i2c_mode 	= I2C_WAIT_SLA_W_ACK;
			tw_if_mode 	= TW_MT;
			
			TWDR 		= dev_w_addr;
			TWCR 		= TWCR_BASE;

		}
		else if (r_data_buf_pos == 0 && r_data_buf_len != 0) {
			i2c_mode	= I2C_WAIT_SLA_R_ACK;
			tw_if_mode	= TW_MR;
	
			TWDR		= dev_r_addr;
			TWCR		= TWCR_BASE;

		}
		else {
			fprintf_P(stderr,PSTR("\n[error] {r,w}_data_buf_pos both not zero.\n"));
		}
	}
	// MASTER TRANSMIT
	else if (tw_stat == TW_MT_SLA_ACK) {
		// sla+w ack, 
		// start writing data

		i2c_mode	= I2C_WAIT_DATA_W_ACK;
		w_data_buf_pos	= 1;

		TWDR		= w_data_buf[0];
		TWCR		= TWCR_BASE;
	}
	else if (tw_stat == TW_MT_SLA_NACK) {
		// sla+w nack
		// restart bus and begin the same transmition again.
		i2c_mode 	= I2C_IDLE;
		tw_if_mode	= TW_MT;

		TWCR		= TWCR_RESET;
	}
	else if (tw_stat == TW_MT_DATA_ACK) {
		// data acked
		// send more data or rep_start to read.
		if (w_data_buf_pos == w_data_buf_len) {
			// Done writing data
			// issue repstart for read
			// FIXME: when read len = zero?
			TWCR		= TWCR_START;
		}
		else {
			// More data to be writen.
			w_data_buf_pos++;
			tw_if_mode = TW_MT;
			TWDR = w_data_buf[w_data_buf_pos];
			TWCR = TWCR_BASE;
		}
	}
	else if (tw_stat == TW_MT_DATA_NACK) {
		// data nacked, rep_start and retransmit.
		i2c_mode 	= I2C_STARTED;
		tw_if_mode	= TW_MT;
		// FIXME: should reset or repstart?
		TWCR		= TWCR_RESET;
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
		fprintf_P(stderr,PSTR("\nSLA+R ACK!"));
		i2c_mode = I2C_READ_DATA;
		TWCR 	 = TWCR_BASE;
	}
	else if (tw_stat == TW_MR_SLA_NACK) {
		// sla+r nack, 
		//???? (rep_start/try again a few times)
		//FIXME: continualy retrys sending read addr.			
		TWDR	= dev_r_addr;
		TWCR 	= TWCR_BASE;
		#if DEBUG_L(1)
		fprintf_P(stderr, PSTR("\n[error] i2c: SLA+R NACK"));
		#endif
	}
	else if (tw_stat == TW_MR_DATA_ACK) {
		// Data read, wait for next read with ack or nack
		r_data_buf[r_data_buf_pos] = TWDR;
		r_data_buf_pos ++;
		if (r_data_buf_pos == r_data_buf_len-1) {
			// One more read to go, send nak
			i2c_mode = I2C_READ_DATA_DONE;
			TWCR = TWCR_NACK;
		}
		else if (r_data_buf_pos >= r_data_buf_len) {
			// No more data to read.
			tw_if_mode	= TW_MT;
			i2c_mode	= I2C_IDLE;
			// call the callback
			if (xfer_complete_cb != NULL)
				xfer_complete_cb();
			TWCR		= TWCR_STOP;
		}
		else {
			// Continue to read data.
			TWCR	 = TWCR_BASE;
		}
	}
	else if (tw_stat == TW_MR_DATA_NACK) {
		// Done transmitting, 
		// check packet length, call the cb
		r_data_buf[r_data_buf_pos] = TWDR;
		r_data_buf_pos ++;
		if (r_data_buf_pos != r_data_buf_len) {
			//FIXME: not enough data read, handle?
			#if DEBUG_L(1)
			fprintf_P(stderr, PSTR("\n[error] i2c: data read	\
				shorter than expected at line %d\n"),__LINE__);
			#endif
		}
		i2c_mode	= I2C_IDLE;
		tw_if_mode	= TW_MT;
	
		//call the callback
		if (xfer_complete_cb != NULL)
			xfer_complete_cb();
		
		TWCR		= TWCR_STOP;
	}
}

