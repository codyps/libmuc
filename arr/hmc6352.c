#include "i2c.h"

/* This style of programming on the i2c bus consumes a considerable amount of memory */
static uint8_t w_buf[] = { 'r' /* 'g' */, 0 /* addr */}; /* read eeprom / read ram */
static uint8_t r_buf[1];
static struct i2c_msg msgs[] = {
	{ HMC6352_ADDR_W, 0, sizeof(w_buf), w_buf },
	{ HMC6352_ADDR_R, 0, sizeof(r_buf), r_buf }
}
static struct i2c_trans cmd = { msgs, sizeof(msgs), hmc6352_cb };


void hmc6352_read_mem(void)
{
		
}
