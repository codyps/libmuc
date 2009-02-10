#ifndef _I2C_HMC6343_H_
#define _I2C_HMC6343_H_

#include <stdint.h>
#include <stdbool.h>
#include "i2c_HMC6343_cmd.h"

#include <stdint.h>
typedef struct {
	union {
		uint16_t head;
		struct {
		uint8_t head_msb;
		uint8_t head_lsb;
		};
	};
	union {
		int16_t pitch;
		struct {
		uint8_t pitch_msb;
		uint8_t pitch_lsb;
		};
	};
	union {
		int16_t roll;
		struct {
		uint8_t roll_msb;
		uint8_t roll_lsb;
		};
	};
} heading_data_t;

heading_data_t head;

volatile bool head_data_updated;

uint8_t hmc6343_cb(void);
void hmc6343_init_static(void);


#endif
