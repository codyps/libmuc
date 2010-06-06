#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "i2c.h"
#include "i2c_HMC6343.h"

static void hmc6343_cb(struct i2c_trans *tran, uint8_t status);

static uint8_t cmd_buf [] = { HMC6343_POST_HEAD };
static uint8_t recv_buf [6];

static struct i2c_msg msgs [] = {
	{ HMC6343_ADDR_W, 0, sizeof(cmd_buf), cmd_buf },
	{ HMC6343_ADDR_R, 0, sizeof(recv_buf), recv_buf }
};

static struct i2c_trans trans = {
	msgs,
	sizeof(msgs) / sizeof(struct i2c_msg),
	hmc6343_cb
};

static void hmc6343_cb(struct i2c_trans *tran, uint8_t status) {
	head.head_msb  = recv_buf[0];
	head.head_lsb  = recv_buf[1];
	head.pitch_msb = recv_buf[2];
	head.pitch_lsb = recv_buf[3];
	head.roll_msb  = recv_buf[4];
	head.roll_lsb  = recv_buf[5];
	
	hmc6343_get_head();
	head_data_updated = true;

//	fprintf_P(io_isr,PSTR("\nhead:%d pitch:%d roll:%d \n"),	head.head,head.pitch,head.roll);
}

void hmc6343_get_head(void) {
	i2c_transfer(&trans);
}
