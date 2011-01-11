#ifndef I2C_SINGLE_H_
#define I2C_SINGLE_H_

#include <util/twi.h>

/** I2C Clock Generation **/
#define F_SCL 100000
#define TWI_PS_MSK 0
#define TWI_BR_VAL -(16*F_SCL-F_CPU)/(2*F_SCL)

/** TWCR Control Values **/
// Enabe TWI, Clear INT flag, Enable Ack, Enable Interupt
#define TWCR_NACK  ( (uint8_t)(1<<TWEN)|(1<<TWINT)|(1<<TWIE) )
#define TWCR_BASE  ( (uint8_t)TWCR_NACK|(1<<TWEA) )
#define TWCR_START ( (uint8_t)TWCR_BASE|(1<<TWSTA) )
#define TWCR_STOP  ( (uint8_t)TWCR_BASE|(1<<TWSTO) )
#define TWCR_RESET ( (uint8_t)TWCR_BASE|(1<<TWSTO)|(1<<TWSTA) )

#endif
