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

void i2c_init(void)
{
	power_twi_enable();

	// Disable TWI hw.
	TWCR = 0;

	// TODO: Setup the pins.
	
	// TODO: Set baud rate.
	TWBR = TWI_BR_VAL;
	TWSR|= TWI_PS_MSK;

	// Slave setup.
	
	// 
}

void i2c_main_handler(void)
{

}

void i2c_xfer(struct i2c_trans *trans)
{
	uint8_t len, twcr = TWCR;
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

	uint8_t twcr = TWCR;
	TWCR = (uint8_t)twcr & (uint8_t)~(1<<TWIE);

	sei();

	switch(tw_status) {
	case TW_START:
	case TW_REP_START:
	case TW_MT_SLA_ACK:
	case TW_MT_DATA_ACK:
	case TW_MR_SLA_ACK:
	case TW_MR_DATA_ACK:

	case TW_MR_DATA_NACK:
	case TW_MR_SLA_NACK:
	case TW_MT_DATA_NACK:
	case TW_MT_SLA_NACK:

	case TW_BUS_ERROR:
	case TW_MT_ARB_LOST:
	default:

	}

	cli();
	TWCR = twcr;
}

