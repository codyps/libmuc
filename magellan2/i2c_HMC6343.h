#ifndef _I2C_HMC6343_H_
#define _I2C_HMC6343_H_

#include <stdint.h>
#include <stdbool.h>
#include "i2c_HMC6343_cmd.h"

volatile struct heading_data_t head;
volatile bool head_data_updated;

uint8_t hmc6343_cb(void);
void hmc6343_init_static(void);


#endif
