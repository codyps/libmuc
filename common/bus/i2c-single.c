// TWI ctrl
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <util/twi.h>

#include "common.h"
#include "i2c.h"
#include "i2c-single.h"

static struct i2c_trans *c_trans;
static uint8_t msg_idx; // current msg in c_trans->msgs[msg_idx].
static uint8_t buf_idx; // current pos in c_trans->msgs[msg_idx]->buf

static bool trans_complete;
static uint8_t trans_status;

#define DEBUG(s, ...) printf_P(PSTR("i2c: " s), ## __VA_ARGS__)
#define TWI_PORT C
#define TWI_SDA_IX 4
#define TWI_SCL_IX 5

void i2c_status(void)
{
	DEBUG("tw_status: %p\n", TW_STATUS);
	DEBUG("c_trans: %p\n", c_trans);
	DEBUG("msg_idx: %d\n", msg_idx);
	DEBUG("buf_idx: %d\n", buf_idx);
	uint8_t port = PIN(TWI_PORT);
	DEBUG("SDA: %d\n", port & (1 << TWI_SDA_IX));
	DEBUG("SCL: %d\n", port & (1 << TWI_SCL_IX));
}

bool i2c_trans_pending(void)
{
	return !!c_trans;
}

void i2c_transfer(struct i2c_trans *trans)
{
	if (unlikely(i2c_trans_pending())) {
		DEBUG("trans pend %p.\n", c_trans);
		return;
	}

	DEBUG("trans start.\n");
	c_trans = trans;
	TWCR = TWCR_START;
}

void i2c_main_handler(void)
{
	if(c_trans && trans_complete) {
		i2c_cb_t scb = c_trans->cb;
		msg_idx = 0;
		buf_idx = 0;
		c_trans = 0;
		trans_status = 0;
		trans_complete = 0;
		if (scb)
			scb(c_trans, trans_status);
	}
}

void i2c_init_master(void)
{
	power_twi_enable();

	// Enable Pullups
	// FIXME: axon specific.
	DDR(TWI_PORT) &= (uint8_t) ~((1<<TWI_SDA_IX)|(1<<TWI_SCL_IX));
	PORT(TWI_PORT) |= ((1<<TWI_SDA_IX)|(1<<TWI_SCL_IX));

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
}

// Debug output for the TWI ISR
#define twi_printf(...)	fprintf(stdout,__VA_ARGS__);
#define twi_printf_P(...) fprintf_P(stdout,__VA_ARGS__);

#define NEXT_MSG() do {                \
	IDEBUG("tw next msg\n");   \
	buf_idx = 0;                   \
	msg_idx++;                     \
	if (msg_idx < c_trans->ct) {   \
		twcr = TWCR_START;     \
	} else {                       \
		trans_status = 0;      \
		trans_complete = true; \
		twcr = TWCR_STOP;      \
	}                              \
} while(0)

#define TW_STOP(_status_) do {         \
	IDEBUG("tw stop\n");       \
	trans_status = _status_;       \
	trans_complete = true;         \
	twcr = TWCR_STOP;              \
} while (0)


#define IDEBUG(s, ...) printf_P(PSTR(s), ## __VA_ARGS__);
ISR(TWI_vect)
{
	uint8_t tw_status = (uint8_t)TW_STATUS;

	/* writing 1 to TWINT causes the bus to proceed,
	 * don't auto clear it */
	uint8_t twcr = (uint8_t)TWCR & (uint8_t)~(1<<TWINT);

	/* disable TWI interrupt and enable global interrupts
	 * to avoid blocking more timing sensitive ISRs */
	TWCR = (uint8_t)twcr & (uint8_t)~(1<<TWIE);
	sei();
	twi_printf("twi_isr %x:", tw_status);
	struct i2c_msg *c_msg = &(c_trans->msgs[msg_idx]);
	switch(tw_status) {
	case TW_START:
	case TW_REP_START:
		/* FIXME: assumes we sent the 'start' */
		IDEBUG("START\n");
		TWDR = c_msg->addr;
		twcr = TWCR_BASE;
		break;

	/** MASTER TRANSMIT **/
	case TW_MT_SLA_ACK:
		IDEBUG("MT_SLA_ACK\n");
		if (c_msg->len) {
			TWDR    = c_msg->buf[0];
			buf_idx = 1;
			twcr    = TWCR_BASE;
		} else {
			/* No data to transmit, so we need to go to
			 * the next msg */
			NEXT_MSG();
		}
		break;
	case TW_MT_DATA_ACK:
		IDEBUG("MT_DATA_ACK\n");
		if (buf_idx < c_msg->len) {
			// more data to transmit.
			TWDR = c_msg->buf[buf_idx];
			buf_idx++;
			twcr = TWCR_BASE;
		} else {
			/* We are done with the present message,
			 * move to the next */
			NEXT_MSG();
		}
		break;

	/** MASTER READ **/
	case TW_MR_SLA_ACK:
		IDEBUG("MR_SLA_ACK\n");
		/* wait for first data packet. */
		if (c_msg->len) {
			twcr = TWCR_BASE;
		} else {
			NEXT_MSG();
		}
		break;

	case TW_MR_DATA_ACK:
		IDEBUG("MR_DATA_ACK\n");
		/* Data read, wait for next read with ack or nack */
		/* this state will not occour without a check of len
		 * to be sure the array write will be defined */
		c_msg->buf[buf_idx] = TWDR;
		buf_idx++;

		if (buf_idx == (c_msg->len - 1)) {
			/* One more read to go, send nak */
			twcr = TWCR_NACK;
		} else if (buf_idx >= c_msg->len) {
			/* No data left for this message, move to next */
			NEXT_MSG();
		} else {
			/* Continue to read data. */
			twcr = TWCR_BASE;
		}
		break;

	case TW_MR_DATA_NACK:
		IDEBUG("MR_DATA_NACK\n");
		/* We nacked the last piece of data. */
		c_msg->buf[buf_idx] = TWDR;
		buf_idx ++;
		if (buf_idx != c_msg->len) {
			/* FIXME: not enough data read, handle? */
			TW_STOP(TW_MR_DATA_NACK);
		} else {
			/* looks like we expected this, and thus
			 * the message is done. move to the next
			 */
			NEXT_MSG();
		}
		break;

	/* NACKs : we don't know how to recover from these,
	 * let the callback deal with it. */
	case TW_MT_SLA_NACK:
	case TW_MR_SLA_NACK:
	case TW_MT_DATA_NACK:
	case TW_BUS_ERROR:
		IDEBUG("NACK||ERROR\n");
		TW_STOP(tw_status);
		break;

	/* case TW_MT_ARB_LOST: */
	case TW_MR_ARB_LOST:
		IDEBUG("ARB_LOST\n");
		/* Send Start when bus becomes free. */
		buf_idx = 0;
		msg_idx = 0;
		twcr = TWCR_START;
		break;

	case TW_NO_INFO:
		IDEBUG("NO_INFO\n");
		//??
		break;

	default:
		IDEBUG("twi_isr: unk twsr %x",tw_status);
		twcr |= (1<<TWINT);
		break;
	}

	cli();
	/* Unstick the TWI hw */
	TWCR = twcr;
}

