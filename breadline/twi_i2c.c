// TWI ctrl
#include <avr/io.h>
#include <avr/twi.h>
#include <avr/power.h>
#include <avr/interrupt.h>

void twi_init(void) {
	power_twi_enable();

	// Disable TWI
	TWCR=0;
	//TWCR&=(uint8_t)~(1<<TWEN);

	// Set SCL Clock
	TWIBR=TWI_BR_VAL;
	TWISR|=TWI_PS_MSK;

	// Set Slave ADDR
	TWAR=I2C_SLAVE_ADDR<<1+I2C_GENERAL_CALL_EN;
	TWAMR=I2C_SLAVE_ADDR_MSK<<1;

	// Enable Acknowledge
	TWCR|=(1<<TWEA);

	// Enable Interupt
	TWCR|=(1<<TWIE);

	// Enable TWI
	TWCR|=(1<<TWEN);
}

ISR(TWI_vect) {
	uint8_t tw_stat = TW_STATUS;
	
	
	// Clear Interrupt Flag
	TWCR|=(1<<TWINT);
}

