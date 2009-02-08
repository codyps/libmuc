
#ifndef _TWI_I2C_H_
#define _TWI_I2C_H_

#include "debug.h"
#include <avr/pgmspace.h>

/** I2C Clock Generation **/
#define F_SCL 100000 //400kHz
#define TWI_PS_MSK 0
#if   (F_SCL == 400000) //400kHz
#define TWI_BR_VAL 2
#elif (F_SCL == 100000) //100kHz
#define TWI_BR_VAL 32
#endif

/** Slave Mode **/
// Slave Address(s) for device  0b7654321
#define I2C_SLAVE_ADDR		0b0011100
#define I2C_SLAVE_ADDR_MSK	0b0000000
// Respond to General Call in Slave mode?
#define I2C_GENERAL_CALL_EN 0




/** State Control **/
typedef enum {TW_MT, TW_MR} tw_if_mode_t;
typedef enum {	I2C_IDLE, 
		I2C_STARTED,
		I2C_SEND_SLA_R,
		I2C_WAIT_SLA_W_ACK,
		I2C_WAIT_SLA_R_ACK,		
		I2C_WAIT_DATA_W_ACK, 
		I2C_WAIT_DATA_R_ACK,
		I2C_READ_DATA,
		I2C_READ_DATA_DONE
	} i2c_mode_t;

/** TWCR Control Values **/
// Enabe TWI, Clear INT flag, Enable Ack, Enable Interupt
#define TWCR_NACK	(1<<TWEN)|(1<<TWINT)|(1<<TWIE)
#define TWCR_BASE	TWCR_NACK|(1<<TWEA)
#define TWCR_START	TWCR_BASE|(1<<TWSTA)
#define TWCR_STOP	TWCR_BASE|(1<<TWSTO)
#define TWCR_RESET	TWCR_BASE|(1<<TWSTO)|(1<<TWSTA)


/** Transfer Control & Queue **/
/*
typedef struct {
	uint8_t addr;
	uint8_t w_len;
	uint8_t r_len;
	uint8_t * w_buf;
	uint8_t * r_buf;
	uint8_t w_pos;
	uint8_t r_pos;
	void * callback;
} i2c_msg_t;

uint8_t i2c_msg_curr;
uint8_t i2c_buf_loc;

#define I2C_MSG_BUF_LEN 5
i2c_msg_t i2c_msg_buffer[I2C_MSG_BUF_LEN];

void i2c_next_msg();

int i2c_add_msg(uint8_t addr,
	uint8_t w_len,
	uint8_t r_len,
	uint8_t * w_buf,
	uint8_t * r_buf, 
	void * callback);
int i2c_rem_msg(int msg_num);
*/

/** Static Bus Operation **/
uint8_t dev_w_addr;
uint8_t dev_r_addr;

uint8_t r_data_buf_len;
uint8_t * r_data_buf;
volatile uint8_t r_data_buf_pos;

uint8_t w_data_buf_len;
uint8_t * w_data_buf;
volatile uint8_t w_data_buf_pos;

void (*xfer_complete_cb)(void);

void twi_init(void);
int i2c_start_xfer(void);



#endif
