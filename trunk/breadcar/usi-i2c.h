// USI as I2C
#ifndef _USI_I2C_H_
#define _USI_I2C_H_


#define DDR_USI	DDRA
#define PORT_USI PORTA
#define PIN_USI_SDA 0
#define PIN_USI_SCL 2


typedef struct {
	uint8_t addr	:7;
	uint8_t rw	:1;
	uint8_t [] data;
	uint8_t data_len;
} i2c_packet_t;


void init_usi_i2c(void);

#endif
