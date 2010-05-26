#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

struct i2c_msg {
	uint8_t addr;
	uint8_t flags;
	void (*cb)(struct i2c_msg *msg);
	uint16_t len;
	uint8_t *buf;
};

void i2c_xfer(struct i2c_msg msgs[], uint8_t ct);


#endif
