
#ifndef _TWI_I2C_H_
#define _TWI_I2C_H_

#define F_SCL 400000 //400kHz

// Slave Address(s) for device
#define I2C_SLAVE_ADDR		0b0011100
#define I2C_SLAVE_ADDR_MSK	0b0000000

// Respond to General Call in Slave mode?
#define I2C_GENERAL_CALL_EN 0


// I2C Clock Generation
#define TWI_PS_MSK 0
#if   (F_SCL == 400000 //400kHz
#define TWI_BR_VAL 2
#elif (F_SCL == 100000 //100kHz
#define TWI_BR_VAL 32
#endif

void twi_init(void);



#endif
