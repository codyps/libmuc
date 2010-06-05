#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/twi.h>

#include "ds/glist.h"
#include "i2c.h"

/** I2C Clock Generation **/
#define F_SCL 100000
#define TWI_PS_MSK 0
#define TWI_BR_VAL -(16*F_SCL-F_CPU)/(2*F_SCL)

/*          name     , fnattr, dattr, data_t          , index_t*/
LIST_DEFINE(i2c_trans, static,      , struct i2c_trans *, uint8_t);
static struct i2c_trans *i2c_trans_buf[4];
static list_t(i2c_trans) i2c_trans_queue = LIST_INITIALIZER(i2c_trans_buf);

static inline void twi_inter_on(void)
{
	TWCR |= (uint8_t) (1<<TWIE);
}

static inline void twi_inter_off(void)
{
	TWCR &= (uint8_t) ~(1<<TWIE);
}

void i2c_init_master(void)
{
	power_twi_enable();

	// Disable TWI hw.
	TWCR = (1<<TWINT); // TWINT is cleared by writing 1 to it.

	// Setup the pins.
	// XXX: move somewhere else?
	// SCL = PC5 / 28 / Analog 5
	// SDA = PC4 / 27 / Analog 4

	// both inputs
	DDRC &= (uint8_t) ~((uint8_t)(1<<PC5)|(1<<PC4));

	// enable internal pull ups.
	PORTC |= (uint8_t)((1<<PC5)|(1<<PC4));

	// Set baud rate.
	TWBR = TWI_BR_VAL;
	TWSR|= TWI_PS_MSK;

	// TWI hw enable.
	TWCR = (uint8_t)(1<<TWEN)|(1<<TWEA)|(1<<TWINT);
}

/* slave_addr:
 *  bits 7:1 set the slave addr (all 0 to disble)
 *  bit 0 set responce to general call.
 */
void i2c_init_slave(uint8_t slave_addr, uint8_t slave_addr_msk) {
	// TODO: Slave setup.
	TWAR = slave_addr;
	TWAMR = slave_addr_msk;
}

void i2c_main_handler(void)
{
	// TODO: check for completed i2c_transfers and call their callbacks.
}

void i2c_transfer(struct i2c_trans *trans)
{
	uint8_t len, twcr = TWCR;

	// temporarily disable TWI interrupt.
	TWCR = (uint8_t)twcr & (uint8_t)~(1<<TWIE);

	len = LIST_CT(i2c_trans_queue);
	list_push(i2c_trans)(&i2c_trans_queue,trans);
	if(len == 0) {
		//TODO: start i2c work.

		twcr |= (uint8_t)(1<<TWIE);
	}

	TWCR = twcr;
}

ISR(TWI_vect)
{
	uint8_t tw_status = TW_STATUS;

	// Temporary disable of TWI isr
	uint8_t twcr = TWCR;
	TWCR = (uint8_t)twcr & (uint8_t)~(1<<TWIE);

	// Enable other interrupts.
	sei();

	switch(tw_status) {
	case TW_START:
	case TW_REP_START:

	case TW_MT_SLA_ACK:
	case TW_MT_SLA_NACK:
	case TW_MT_DATA_ACK:
	case TW_MT_DATA_NACK:

	//case TW_MT_ARB_LOST: // identical to the following:
	case TW_MR_ARB_LOST:
	
	case TW_MR_SLA_ACK:
	case TW_MR_SLA_NACK:

	case TW_MR_DATA_ACK:
	case TW_MR_DATA_NACK:
	
	case TW_ST_SLA_ACK:
	case TW_ST_ARB_LOST_SLA_ACK:
	case TW_ST_DATA_ACK:
	case TW_ST_DATA_NACK:
	case TW_ST_LAST_DATA:
	case TW_SR_SLA_ACK:
	case TW_SR_ARB_LOST_SLA_ACK:

	case TW_SR_GCALL_ACK:
	case TW_SR_ARB_LOST_GCALL_ACK:
	case TW_SR_DATA_ACK:
	case TW_SR_DATA_NACK:
	case TW_SR_GCALL_DATA_ACK:
	case TW_SR_GCALL_DATA_NACK:
	case TW_SR_STOP:
	case TW_NO_INFO:

	case TW_BUS_ERROR:
	default:
		break;
	}

	// Change things back to normal.
	cli();
	TWCR = twcr;
}

