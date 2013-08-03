#include "config.h"
#include <stdbool.h>

#include <avr/power.h>
#include <avr/interrupt.h>

#include "twi2.h"
#include "twi_common.h"
#include "twi_calc.h"

#ifndef TWI_ERR
#warning "twi: TWI_ERR(val, fmt, ...) is not defined, you will not be notified of errors."
#define TWI_ERR(val, fmt, ...)
#endif

static i2c_trans     *trans;
static uint8_t       trans_msg_ct; /* the number of messages that have completed transmission */
static i2c_msg *msg; /* points to the current message being processed */
static i2c_master_cb master_cb;

/* Set to UINT8_MAX when the address has yet to be sent. Otherwise indicates
 * the message index of the byte that is about to be sent. */
static uint8_t msg_pos;
static uint8_t *msg_byte; /* pointer to the byte to be read */
static uint8_t msg_len; /* limit on length of msg_pos */

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

static uint8_t trans_msg_len(i2c_msg *pos)
{
	uint8_t *curr_msg = (uint8_t *)pos;
	return !twi_addr_is_write(*curr_msg)
		? TWI_RMSG_IS_EMBEDDED(curr_msg)
			? TWI_SZ_ERMSG(curr_msg[TWI_ERMSG_LEN_IX])
			: TWI_SZ_IRMSG
		: TWI_WMSG_IS_EMBEDDED(curr_msg)
			? TWI_SZ_EWMSG(curr_msg[TWI_EWMSG_LEN_IX])
			: TWI_SZ_IWMSG;
}

static uint8_t *trans_msg_byte_addr(i2c_msg *pos)
{
	uint8_t *curr_msg = (uint8_t *)pos;
	if (twi_addr_is_write(pos->addr)) {
		if (TWI_RMSG_IS_EMBEDDED(curr_msg)) {
			return ((struct i2c__embedded_msg *)pos)->data;
		} else {
			return ((struct i2c__indirect_msg *)pos)->data;
		}
	} else {
		if (TWI_WMSG_IS_EMBEDDED(curr_msg)) {
			return ((struct i2c__embedded_msg *)pos)->data;
		} else {
			return ((struct i2c__indirect_msg *)pos)->data;
		}
	}
}

static i2c_msg *trans_next_msg(i2c_msg *pos)
{
	return (i2c_msg *)((uint8_t *)pos + trans_msg_len(pos));
}

static uint8_t msg_addr(i2c_msg *pos)
{
	return pos->addr;
}

void twi_service(void)
{
	if (state <= S_STOPPED && state != S_NONE) {
		master_cb(trans, msg, state - S_STOPPED);
		UPDATE_STATE(S_NONE);
	}
}

void twi_init_master(void)
{
	power_twi_enable();

	TWSR = TWI_TWPS_FROM_FREQ(F_CPU, F_SCL);
	TWBR = TWI_TWBR_FROM_FREQ(F_CPU, F_SCL);
}

static void reset_to_start_of_msg(void)
{
	msg_pos = UINT8_MAX;
	msg_len  = trans_msg_len(msg);
	msg_byte = trans_msg_byte_addr(msg);
}

static void reset_to_start_of_trans(void)
{
	msg = (i2c_msg *)&trans->__data;
	trans_msg_ct = 0;
	reset_to_start_of_msg();
}

void twi_xfer(i2c_trans *t, i2c_master_cb cb, void *arg)
{
	if (BUG_IF_STATE_NOT(S_NONE))
		return;

	trans     = t;
	master_cb = cb;
	reset_to_start_of_trans();
	UPDATE_STATE(S_STARTING);
	TWCR = TWCR_START;
}

#define STOP(state) do {	\
	UPDATE_STATE(state);	\
	TWCR = TWCR_STOP;	\
} while (0)

static void next_message(void)
{
	if (trans_msg_ct < trans->ct) {
		msg = trans_next_msg(msg);
		trans_msg_ct ++;
		reset_to_start_of_msg();
		UPDATE_STATE(S_STARTING);
		TWCR = TWCR_START;
	} else {
		STOP(S_STOPPED);
	}
}

static bool current_trans_is_write(void)
{
	return twi_addr_is_write(msg_addr(msg));
}

ISR(TWI_vect)
{
	uint8_t status = TW_STATUS;
	switch (status) {
	/* Master */
	case TW_REP_START: /* we have transmitted a repeated start */
		BUG_IF_STATE_NOT(S_STARTING);
		/* repeats can only happend after at least 1 complete msg has transmitted */
		BUG_ON(msg == 0);

		/* hw */
		TWDR = msg_addr(msg);
		UPDATE_STATE(S_SENT_ADDR);
		break;

	case TW_START: /* we have transmitted a start */
		BUG_IF_STATE_NOT(S_STARTING);
		BUG_ON(msg != 0);

		/* hw */
		TWDR = msg_addr(msg);
		UPDATE_STATE(S_SENT_ADDR);
		break;

	/* Master Transmitter */
	case TW_MT_SLA_ACK: /* SLA+W transmitted, ACK received */
		BUG_ON_CTRANS_READ();
		BUG_IF_STATE_NOT(S_SENT_ADDR);
		BUG_ON(msg_pos != UINT8_MAX);

		if (msg_len > 0) { // > msg_pos)
			TWDR = *msg_byte; // [msg_pos];
			msg_pos = 0; // msg_pos ++;
			msg_byte++;
			UPDATE_STATE(S_SENT_DATA);
		} else {
			next_message();
		}
		break;

	case TW_MT_SLA_NACK: /* SLA+W transmitted, NACK received */
		BUG_ON_CTRANS_READ();
		BUG_IF_STATE_NOT(S_SENT_ADDR);
		/* FIXME: pass control to callback for REPSTART or STOP */
		UPDATE_STATE(S_NACK_ADDR_W);
		TWCR = TWCR_STOP;
		break;

	case TW_MT_DATA_ACK: /* data transmitted, ACK received */
		BUG_ON_CTRANS_READ();
		BUG_IF_STATE_NOT(S_SENT_DATA);

		if (msg_pos < msg_len) {
			TWDR = *msg_byte;
			msg_byte++;
			msg_pos ++;
			UPDATE_STATE(S_SENT_DATA);
		} else {
			next_message();
		}
		break;

	case TW_MT_DATA_NACK: /* data transmitted, NACK received */
		BUG_ON_CTRANS_READ();
		BUG_IF_STATE_NOT(S_SENT_DATA);
		/* FIXME: pass control to callback for REPSTART or STOP */
		UPDATE_STATE(S_NACK_DATA_W);
		TWCR = TWCR_STOP;
		break;

	/* Master (these have the same value) */
	//case TW_MT_ARB_LOST: /* arbitration lost in SLA+W or data */
	case TW_MR_ARB_LOST: /* arbitration lost in SLA+R or NACK */
		BUG_ON(STATE_IS_NOT(S_SENT_DATA) && STATE_IS_NOT(S_SENT_ADDR) && STATE_IS_NOT(S_WAIT_FOR_BYTE_NACK));
		/* XXX: should we defer to a callback? */
		UPDATE_STATE(S_ARB);
		TWCR = TWCR_STOP;
		break;

	/* Master Receiver */
	case TW_MR_SLA_ACK: /* SLA+R transmitted, ACK received */
		BUG_ON_CTRANS_WRITE();
		BUG_IF_STATE_NOT(S_SENT_ADDR);

		/* decide whether to ACK next recieved packet */
		if (msg_len == 0) {
			/* A read with no bytes? FUNKY. */
			next_message();
		} else if (msg_len == 1) {
			UPDATE_STATE(S_WAIT_FOR_BYTE_NACK);
			msg_pos = 0; /* ++ */
			TWCR = TWCR_NACK;
		} else {
			UPDATE_STATE(S_WAIT_FOR_BYTE);
			msg_pos = 0; /* ++ */
			TWCR = TWCR_ACK;
		}
		break;

	case TW_MR_SLA_NACK: /* SLA+R transmitted, NACK received */
		BUG_ON_CTRANS_WRITE();
		BUG_IF_STATE_NOT(S_SENT_ADDR);
		/* FIXME: pass control to callback for REPSTART or STOP */
		UPDATE_STATE(S_NACK_ADDR_R);
		TWCR = TWCR_STOP;
		break;

	case TW_MR_DATA_ACK: /* data received, ACK returned */
		BUG_ON_CTRANS_WRITE();
		BUG_IF_STATE_NOT(S_WAIT_FOR_BYTE);
		/* Last recieved byte should be processed in NACK */
		BUG_ON(msg_pos + 1 == msg_len);
		BUG_ON(msg_pos + 1 >  msg_len);

		*msg_byte = TWDR;
		msg_byte++;
		msg_pos ++;

		/* decide whether to ACK next recieved packet */
		if (msg_pos + 1 == msg_len) {
			//assume: UPDATE_STATE(S_WAIT_FOR_BYTE);
			TWCR = TWCR_NACK;
		} else {
			UPDATE_STATE(S_WAIT_FOR_BYTE_NACK);
			TWCR = TWCR_NACK;
		}
		break;

	case TW_MR_DATA_NACK: /* data received, NACK returned */
		BUG_ON_CTRANS_WRITE();
		BUG_IF_STATE_NOT(S_WAIT_FOR_BYTE_NACK);
		BUG_ON((msg_pos + 1) != msg_len);
		/* This state only occurs if we purposely return a NACK.
		 * We only return a NACK if this is the last byte to read */
		*msg_byte = TWDR;
		msg_byte ++;
		msg_pos  ++;

		next_message();
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
