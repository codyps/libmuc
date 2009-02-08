#ifndef _I2C_HMC_
#define _I2C_HMC_

#include <stdint.h>
#include "i2c_HMC6343_cmd.h"

void hmc6343_cb(void);

/** Variables for Static operation of the bus **/

uint8_t dev_w_addr = HMC6343_ADDR_W;
uint8_t dev_r_addr = HMC6343_ADDR_R;

#define r_data_buf_len 6
uint8_t r_data_buf[r_data_buf_len];
volatile uint8_t r_data_buf_pos; //=0

const uint8_t w_data_buf_len = 1;
uint8_t w_data_buf[1] = {0x50};
volatile uint8_t w_data_buf_pos; //=0

void (*xfer_complete_cb)(void) = &hmc6343_cb;

#endif
