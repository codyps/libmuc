/*
	I2C Bitbang for attiny861
	Supported (Master Mode): Syncronization, Arbitration, Clock Streching
 	Todo: Slave mode.
*/
#ifndef _I2C_MASTER_BB_H_
#define _I2C_MASTER_BB_H_

// Hardware Locations
#define I2C_PORT PORTA 
#define I2C_PIN  PINA
#define I2C_DDR  DDRA

#define I2C_SDA  0
#define I2C_SCL  2

#define I2C_PULLUPS 1

// Bus Speed
enum {I2C_SM,I2C_FM,I2C_FMP};
#define I2C_SPEED I2C_SM

#if (I2C_SPEED==I2C_SM)
#define I2C_T_LOW  4.7
#define I2C_T_HIGH 4.0
#define I2C_T_SU_STA 4.7
#define I2C_T_HD_STA 4.0
#define I2C_T_SU_STO 4.0

#elif (I2C_SPEED==I2C_FM)
#define I2C_T_LOW  1.3
#define I2C_T_HIGH 0.6
#define I2C_T_SU_STA 0.6
#define I2C_T_HD_STA 0.6
#define I2C_T_SU_STO 0.6

#elif (I2C_SPEED==I2C_FMP)
#define I2C_T_LOW  0.5
#define I2C_T_HIGH 0.26
#define I2C_T_SU_STA 0.26
#define I2C_T_HD_STA 0.26
#define I2C_T_SU_STO 0.26
#endif

enum {I2C_MODE_WRITE=0, I2C_MODE_READ=1 };

// i2c_write return values:
#define I2C_ACK	 	0b00
#define I2C_NACK 	0b01
#define I2C_BUS_LOSS 	0b11 // Loosing arbitration on MM.

// i2c_transfer return masks:
#define I2C_TRANS_COMP	0b0000 // Finished specified trans len.
#define I2C_DEVICE_NACK	0b0100 // Device never acked, | with i2c_write ret.
#define I2C_TRANS_END	0b1000 // NACK before specified trans len.
#define I2C_ZERO_LEN	0b1100


static inline void i2c_line_high_bb(uint8_t line);
static inline void i2c_line_low_bb(uint8_t line);

int8_t i2c_write_bb(uint8_t data);
uint8_t i2c_read_bb(int8_t ack);

int8_t i2c_trans_bb(uint8_t addr, uint8_t len, uint8_t * data);
int8_t i2c_vtrans_bb(uint8_t addr, uint8_t len, ...);
int8_t i2c_vatrans_bb(uint8_t addr, uint8_t len, va_list  list); 

int8_t i2c_command_bb(uint8_t addr,				\
			uint8_t arg_len, uint8_t * args , 	\
			uint8_t ret_len, uint8_t * ret);
int8_t i2c_vcommand_bb(uint8_t addr, uint8_t cmd, uint8_t arg_len, uint8_t ret_len, ...);

void i2c_start_bb(void);
void i2c_restart_bb(void);
void i2c_stop_bb(void);
void i2c_reset_bb(void);

#endif
