#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>


#include "twi_i2c.h"
#include "i2c_HMC6343.h"

#define data_buf_len 6
uint8_t data_buf[data_buf_len];
uint8_t cmd;




uint8_t hmc6343_cb(void) {
	//FIXME: 
	memcpy(&head , data_buf, sizeof(head));
	
	w_data_buf_pos = 0;
	r_data_buf_pos = 0;
	head_data_updated = true;
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
	
	xfer_complete_cb = &hmc6343_cb;
	head_data_updated = false;
}


