#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "i2c-single.h"
#include "i2c_HMC6343.h"

#define data_buf_len 6
uint8_t data_buf[data_buf_len];
uint8_t cmd;

uint8_t hmc6343_cb(void) {
	head.head_msb  = data_buf[0];
	head.head_lsb  = data_buf[1];
	head.pitch_msb = data_buf[2];
	head.pitch_lsb = data_buf[3];
	head.roll_msb  = data_buf[4];
	head.roll_lsb  = data_buf[5];
	
	hmc6343_init_static();
	head_data_updated = true;

//	fprintf_P(io_isr,PSTR("\nhead:%d pitch:%d roll:%d \n"),	head.head,head.pitch,head.roll);
	return TWCR_STOP;
}

void hmc6343_init_static(void) {
	dev_w_addr = HMC6343_ADDR_W;
	dev_r_addr = HMC6343_ADDR_R;

	r_data_buf_len = data_buf_len;
	r_data_buf = data_buf;
	r_data_buf_pos = 0;
	
	w_data_buf_len = 1;
	cmd = HMC6343_POST_HEAD;
	w_data_buf = &cmd;
	w_data_buf_pos = 0;
	
	xfer_complete_cb = hmc6343_cb;
	head_data_updated = false;
}


