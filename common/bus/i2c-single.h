#ifndef I2C_SINGLE_H_
#define I2C_SINGLE_H_

#include <util/twi.h>

/** I2C Clock Generation **/
#define F_SCL 100000
#define TWI_PS_MSK 0
#define TWI_BR_VAL -(16*F_SCL-F_CPU)/(2*F_SCL)

/** Slave Mode **/
// Slave Address(s) for device  0b7654321
#define I2C_SLAVE_ADDR		0b0011100
#define I2C_SLAVE_ADDR_MSK	0b0000000
// Respond to General Call in Slave mode?
#define I2C_GENERAL_CALL_EN 0

/** State Control **/
typedef enum {	I2C_IDLE = 0, 
		I2C_BUSY = 1,
		I2C_MT = 2,
		I2C_MR = 3,
		I2C_ST = 4,
		I2C_SR = 5
} i2c_mode_t;
i2c_mode_t i2c_mode;


/** TWCR Control Values **/
// Enabe TWI, Clear INT flag, Enable Ack, Enable Interupt
#define TWCR_NACK  ( (uint8_t)(1<<TWEN)|(1<<TWINT)|(1<<TWIE) )
#define TWCR_BASE  ( (uint8_t)TWCR_NACK|(1<<TWEA) )
#define TWCR_START ( (uint8_t)TWCR_BASE|(1<<TWSTA) )
#define TWCR_STOP  ( (uint8_t)TWCR_BASE|(1<<TWSTO) )
#define TWCR_RESET ( (uint8_t)TWCR_BASE|(1<<TWSTO)|(1<<TWSTA) )

/** Static Bus Operation **/
uint8_t dev_w_addr;
uint8_t dev_r_addr;

uint8_t w_data_buf_len;
uint8_t * w_data_buf;
volatile uint8_t w_data_buf_pos;

uint8_t r_data_buf_len;
volatile uint8_t * r_data_buf;
volatile uint8_t r_data_buf_pos;

uint8_t (*xfer_complete_cb)(void);

/** Functions **/
void i2c_start_xfer(void);

#endif
