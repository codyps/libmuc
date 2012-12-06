#include "bus/i2c.h"
#include "ex/i2c_HMC6352_cmd.h"
#include "hmc6352.h"

#include <avr/pgmspace.h>

#include <stdio.h>
#include <stdint.h>

/* This style of programming on the i2c bus consumes a considerable
 * amount of memory
 */
#define DEBUG(x , ...) printf_P(PSTR("hmc6352: " x), ## __VA_ARGS__)

/* read eeprom / read ram */
static uint8_t w_buf[] = { 'r' /* 'g' */, 0 /* addr */};
static uint8_t r_buf[1];
static struct i2c_msg msgs[] = {
	I2C_MSG(HMC6352_ADDR_W, w_buf),
	I2C_MSG(HMC6352_ADDR_R, r_buf)
};
static struct i2c_trans cmd = I2C_TRANS(msgs, 0);

static uint8_t last;

static void read_multi_cb(struct i2c_trans *trans, uint8_t status)
{
	if (status) {
		DEBUG("i2c error: %d\n", status);
		return;
	}

	DEBUG("%x => %x\n", w_buf[1], r_buf[0]);

	if (w_buf[1] < last) {
		w_buf[1] ++;
		i2c_transfer(&cmd);
	}
}

void hmc6352_read_all_mem(void)
{
	if (i2c_trans_pending()) {
		DEBUG("already attempting com.\n");
		return;
	}

	DEBUG("read mem.\n");
	w_buf[1] = 0;
	last = 0xff;
	cmd.cb = read_multi_cb;
	i2c_transfer(&cmd);
}

static void read_one_cb(struct i2c_msg *msgs, uint8_t msg_ct, uint8_t msg_pos, void *data, uint8_t status)
{
	if (status) {
		DEBUG("i2c error: %d\n", status);
		return;
	}

	DEBUG("%x => %x\n", w_buf[1], r_buf[0]);
}

void hmc6352_read_mem(uint8_t addr)
{
	if (i2c_trans_pending()) {
		DEBUG("already attempting com.\n");
		return;
	}

	DEBUG("read mem.\n");
#if 0
	w_buf[1] = addr;
	cmd.cb = read_one_cb;
	i2c_transfer(&cmd);
#endif
	char *trans = malloc(TWI_SZ_BASE + TWI_SZ_WMSG(1) + TWI_SZ_RIMSG(1));
	twi_start_msg(trans, HMC6352_ADDR_W);
	twi_send(trans, 'r');
	twi_end_msg(trans);
	twi_start_msg(trans, HMC6352_ADDR_R);
	twi_recv(trans, 1);
	twi_end_msg(trans);

	i2c_xfer(trans, read_one_cb)
}

