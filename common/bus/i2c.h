#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>

struct i2c_msg {
	uint8_t addr;
	uint8_t flags;
	uint16_t len;
	uint8_t *buf;
};

#define I2C_MSG(addr, buf) { addr, 0, sizeof(buf), buf }
#define I2C_TRANS(msgs,cb) { msgs, sizeof(msgs)/sizeof(*msgs), cb }

/* an i2c "transaction" */
struct i2c_trans {
	struct i2c_msg *msgs;
	uint8_t ct;
	void (*cb)(struct i2c_trans *trans, uint8_t status);
};

void i2c_init_master(void);
void i2c_init_slave(uint8_t slave_addr, uint8_t slave_addr_msk);
void i2c_main_handler(void); /* main loop context manager. */

void i2c_transfer(struct i2c_trans *tran);

#endif
