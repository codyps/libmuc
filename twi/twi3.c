#include "config.h"
#include <stdbool.h>

#include <avr/power.h>
#include <avr/interrupt.h>

#include "twi2.h"
#include "twi2_calc.h"

#ifndef TWI_ERR
#warning "twi: TWI_ERR(val, fmt, ...) is not defined, you will not be notified of errors."
#define TWI_ERR(val, fmt, ...)
#endif

#ifndef F_SCL
#define F_SCL KILO(100ULL) /* default to 100Khz */
#endif

static i2c_master_cb master_cb;
static void         *cb_data;
static uint8_t	     cur_addr;

/* Set to UINT8_MAX when the address has yet to be sent. */
static uint8_t msg_pos;

static enum i2c_state {
	S_NONE,
	S_ARB,	       // failure states
	S_NACK_ADDR_W, // |
	S_NACK_ADDR_R, // |
	S_NACK_DATA_W, // +-
	S_STOPPED, /* transaction completed */
#ifdef DEBUG_I2C_TRACK_STATES
	S_STARTING,
	S_SENT_ADDR,
	S_SENT_DATA,
	S_WAIT_FOR_BYTE,
	S_WAIT_FOR_BYTE_NACK,
#endif
} state;

#ifdef DEBUG_I2C_TRACK_STATES
# define UPDATE_STATE(___state) do { state = (___state); } while(0)
# define BUG_IF_STATE_NOT(___state) BUG_ON((___state) != state)
# define STATE_IS_NOT(___state) (state == (___state))
#else
# error
# define UPDATE_STATE(___state) do {	\
	if (___state <= S_STOPPED)	\
		state = ___state;	\
} while(0)
# define BUG_IF_STATE_NOT(___state) ((void)___state, false)
# define STATE_IS_NOT(___state)     ((void)___state, false)
#endif

#ifdef DEBUG_I2C_CHECK_ADDR
# define BUG_ON_CTRANS_WRITE() BUG_ON(current_trans_is_write())
# define BUG_ON_CTRANS_READ()  BUG_ON(!current_trans_is_write())
#else
# error
# define BUG_ON_CTRANS_WRITE() false
# define BUG_ON_CTRANS_READ()  false
#endif

bool twi_addr_is_write(uint8_t addr)
{
	return !!(addr & 1);
}

bool current_trans_is_write(void)
{
	return twi_addr_is_write(cur_addr);
}

void twi_init_master(void)
{
	power_twi_enable();

	TWSR = TWI_TWPS_FROM_FREQ(F_CPU, F_SCL);
	TWBR = TWI_TWBR_FROM_FREQ(F_CPU, F_SCL);
}

#if 0
static void reset_pos(void)
{
	msg_pos = UINT8_MAX;
}
#endif

void twi_byte_xfer(uint8_t addr, i2c_master_cb cb, void *arg)
{
	if (BUG_IF_STATE_NOT(S_NONE))
		return;

	master_cb = cb;
	//reset_pos();
	UPDATE_STATE(S_STARTING);
	TWCR = TWCR_START;
}

#if 0
static void next_message(void)
{
	if (trans_pos < trans.ct) {
		trans_pos = trans_next_msg(trans_pos);
		msg_pos = UINT8_MAX;
		UPDATE_STATE(S_STARTING);
		TWCR = TWCR_START;
	} else {
		UPDATE_STATE(S_STOPPED);
		reset_pos();
		TWCR = TWCR_STOP;
	}
}
#endif

ISR(TWI_vect)
{
	uint8_t status = TW_STATUS;
	switch (status) {
	/* Master */
	case TW_START: /* we have transmitted a start */
	case TW_REP_START: /* we have transmitted a repeated start */
		BUG_IF_STATE_NOT(S_STARTING);

		/* hw */
		TWDR = cur_addr;
		TWCR = TWCR_BASE;
		UPDATE_STATE(S_SENT_ADDR);
		break;

	/* Master Transmitter */
	case TW_MT_SLA_ACK: /* SLA+W transmitted, ACK received */
		BUG_ON_CTRANS_READ();
		BUG_IF_STATE_NOT(S_SENT_ADDR);
		BUG_ON(msg_pos != UINT8_MAX);

		SET_VALID_ACTIONS(TWI_ACT_REPSTART | TWI_ACT_STOP | TWI_ACT_SEND_DATA);
		master_cb(cb_data, status);
#if 0
		if (trans.msgs[trans_pos].len > 0) { // > msg_pos)
			msg_pos = 0; // msg_pos ++;
			TWDR = trans.msgs[trans_pos].data[0]; // [msg_pos];
			UPDATE_STATE(S_SENT_DATA);
		} else {
			next_message();
		}
#endif
		break;

	case TW_MT_SLA_NACK: /* SLA+W transmitted, NACK received */
		BUG_ON_CTRANS_READ();
		BUG_IF_STATE_NOT(S_SENT_ADDR);
		UPDATE_STATE(S_NACK_ADDR_W);

		/* pass control to callback for REPSTART or STOP */
		SET_VALID_ACTIONS(TWI_ACT_REPSTART | TWI_ACT_STOP);
		master_cb(cb_data, status);
		break;

	case TW_MT_DATA_ACK: /* data transmitted, ACK received */
		BUG_ON_CTRANS_READ();
		BUG_IF_STATE_NOT(S_SENT_DATA);

		SET_VALID_ACTIONS(TWI_ACT_REPSTART | TWI_ACT_STOP | TWI_ACT_SEND_DATA);
		master_cb(cb_data, status);
#if 0
		msg_pos ++;
		if (msg_pos < trans.msgs[trans_pos].len) {
			TWDR = trans.msgs[trans_pos].data[msg_pos];
			UPDATE_STATE(S_SENT_DATA);
		} else {
			next_message();
		}
#endif
		break;

	case TW_MT_DATA_NACK: /* data transmitted, NACK received */
		BUG_ON_CTRANS_READ();
		BUG_IF_STATE_NOT(S_SENT_DATA);
		UPDATE_STATE(S_NACK_DATA_W);

		/* pass control to callback for REPSTART or STOP */
		SET_VALID_ACTIONS(TWI_ACT_REPSTART | TWI_ACT_STOP);
		master_cb(cb_data, status);
		break;

	/* Master (these have the same value for transmit & recieve) */
	//case TW_MT_ARB_LOST: /* arbitration lost in SLA+W or data */
	case TW_MR_ARB_LOST: /* arbitration lost in SLA+R or NACK */
		BUG_ON(STATE_IS_NOT(S_SENT_DATA) && STATE_IS_NOT(S_SENT_ADDR)
				&& STATE_IS_NOT(S_WAIT_FOR_BYTE_NACK));
		UPDATE_STATE(S_ARB);

		SET_VALID_ACTIONS(TWI_ACT_REPSTART | TWI_ACT_STOP);
		master_cb(cb_data, status);

#if 0
		reset_pos();
#endif
		break;

	/* Master Receiver */
	case TW_MR_SLA_ACK: /* SLA+R transmitted, ACK received */
		BUG_ON_CTRANS_WRITE();
		BUG_IF_STATE_NOT(S_SENT_ADDR);

		/* decide whether to ACK next recieved packet */
		SET_VALID_ACTIONS(TWI_ACT_ACK | TWI_ACT_NACK);
		master_cb(cb_data, status);

#if 0
		if (trans.msgs[trans_pos].len == 0) {
			/* A read with no bytes? FUNKY. */
			next_message();
		} else if (trans.msgs[trans_pos].len == 1) {
			UPDATE_STATE(S_WAIT_FOR_BYTE_NACK);
			msg_pos = 0;
			TWCR = TWCR_NACK;
		} else {
			UPDATE_STATE(S_WAIT_FOR_BYTE);
			msg_pos = 0;
			TWCR = TWCR_ACK;
		}
#endif
		break;

	case TW_MR_SLA_NACK: /* SLA+R transmitted, NACK received */
		BUG_ON_CTRANS_WRITE();
		BUG_IF_STATE_NOT(S_SENT_ADDR);
		UPDATE_STATE(S_NACK_ADDR_R);

		/* pass control to callback for REPSTART or STOP */
		SET_VALID_ACTIONS(TWI_ACT_REPSTART | TWI_ACT_STOP);
		master_cb(cb_data, status);
		break;

	case TW_MR_DATA_ACK: /* data received, ACK returned */
		BUG_ON_CTRANS_WRITE();
		BUG_IF_STATE_NOT(S_WAIT_FOR_BYTE);

		SET_VALID_ACTIONS(TWI_ACT_ACK | TWI_ACT_NACK);
		master_cb(cb_data, status);

#if 0
		/* Last recieved byte should be processed in NACK */
		BUG_ON(msg_pos + 1 == trans.msgs[trans_pos].len);
		BUG_ON(msg_pos + 1 >  trans.msgs[trans_pos].len);

		trans.msgs[trans_pos].data[msg_pos] = TWDR;

		msg_pos ++;
		/* decide whether to ACK next recieved packet */
		if (msg_pos + 1 == trans.msgs[trans_pos].len) {
			UPDATE_STATE(S_WAIT_FOR_BYTE_NACK);
			TWCR = TWCR_NACK;
		} else {
			//assume: UPDATE_STATE(S_WAIT_FOR_BYTE);
			TWCR = TWCR_ACK;
		}
#endif
		break;

	case TW_MR_DATA_NACK: /* data received, NACK returned */
		BUG_ON_CTRANS_WRITE();
		BUG_IF_STATE_NOT(S_WAIT_FOR_BYTE_NACK);
		/* This state only occurs if we purposely return a NACK.
		 * We only return a NACK if this is the last byte to read */

		SET_VALID_ACTIONS(TWI_ACT_ACK | TWI_ACT_NACK);
		master_cb(cb_data, status);
#if 0
		BUG_ON((msg_pos + 1) != trans.msgs[trans_pos].len);
		trans.msgs[trans_pos].data[msg_pos] = TWDR;
		next_message();
#endif
		break;

	/* Misc */
	case TW_NO_INFO:
	case TW_BUS_ERROR:
		TWI_ERR(status, "error.");
		break;

	/* Slave Transmitter */
	case TW_ST_SLA_ACK: /* SLA+R received, ACK returned */
	case TW_ST_ARB_LOST_SLA_ACK: /* arbitration lost in SLA+RW, SLA+R received, ACK returned */
	case TW_ST_DATA_ACK: /* data transmitted, ACK received */
	case TW_ST_DATA_NACK: /* data transmitted, NACK received */
	case TW_ST_LAST_DATA: /* last data byte transmitted, ACK received */

	/* Slave Receiver */
	case TW_SR_SLA_ACK: /* SLA+W received, ACK returned */
	case TW_SR_ARB_LOST_SLA_ACK: /* arbitration lost in SLA+RW, SLA+W received, ACK returned */
	case TW_SR_GCALL_ACK: /* general call received, ACK returned */
	case TW_SR_ARB_LOST_GCALL_ACK: /* arbitration lost in SLA+RW, general call received, ACK returned */
	case TW_SR_DATA_ACK: /*data received, ACK returned */
	case TW_SR_DATA_NACK: /* data received, NACK returned */
	case TW_SR_GCALL_DATA_ACK: /* general call data received, ACK returned */
	case TW_SR_GCALL_DATA_NACK: /* general call data received, NACK returned */

	default:
		TWI_ERR(status | 0x01, "unknown.");
		break;
	}
}
