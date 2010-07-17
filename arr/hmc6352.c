#include "bus/i2c.h"
#include "ex/i2c_HMC6352_cmd.h"
#include "hmc6352.h"

#include <stdio.h>
#include <stdint.h>

/* This style of programming on the i2c bus consumes a considerable
 * amount of memory 
 */

/* read eeprom / read ram */
static uint8_t w_buf[] = { 'r' /* 'g' */, 0 /* addr */};
static uint8_t r_buf[1];
static struct i2c_msg msgs[] = {
	I2C_MSG(HMC6352_ADDR_W, w_buf),
	I2C_MSG(HMC6352_ADDR_R, r_buf)
};

static void read_b_cb(struct i2c_trans *trans, uint8_t status);
static struct i2c_trans cmd = I2C_TRANS(msgs, read_b_cb);

static uint8_t last;

static void read_b_cb(struct i2c_trans *trans, uint8_t status)
{
	printf("hmc6352: callback\n");
	if (status) {
		printf("i2c error: %d\n", status);
		return;
	}

	printf("i2c: %d => %d\n", w_buf[1], r_buf[0]);

	if (w_buf[1] < last) {
		w_buf[1] ++;
		i2c_transfer(&cmd);
	}
}

void hmc6352_read_mem(void)
{
	printf("hmc6352: read mem.\n");
	w_buf[1] = 0;
	last = 0xff;
	i2c_transfer(&cmd);
}
