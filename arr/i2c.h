#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

// an i2c "transaction"
struct i2c_trans {
	struct i2c_msg *msgs;
	uint8_t ct;
	void (*cb)(struct i2c_msg *msg);
};

struct i2c_msg {
	uint8_t addr;
	uint8_t flags;
	uint16_t len;
	uint8_t *buf;
};

void i2c_init(void); // initialization.
void i2c_main_handler(void); // main loop context manager.

void i2c_xfer(struct i2c_trans *tran);

#endif
