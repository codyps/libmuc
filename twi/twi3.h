/* TWI Library (per byte callbacks)
 *
 * The user supplies a callback for use on each byte recv or send.
 * This callback also is given control of errors.
 *
 * Callback runs in isr context.
 */

/* XXX: does this setup even make sense?
 *      What work are we actually taking away from the callback?
 *      It appears that the ISR switch just fiddles with state and then passes
 *      control to the callback, which means the callback needs it's own giant
 *      switch. Which is silly & annoying. */

#ifndef TWI3_H_
#define TWI3_H_

#include <stdint.h>
#include <stdbool.h>
#include "twi_common.h"

/* Actions for use in the callback passwd to twi_byte_xfer() */
/* XXX: FIXME: thse need to update the state */
#define TWI_DO_REPSTART() do {				\
	BUG_IF_NOT_VALID_ACTION(TWI_ACT_REPSTART);	\
	TWCR = TWCR_START;				\
} while(0)

#define TWI_DO_NACK() do {				\
	BUG_IF_NOT_VALID_ACTION(TWI_ACT_NACK);		\
	TWCR = TWCR_NACK;				\
} while(0)

#define TWI_DO_ACK() do {				\
	BUG_IF_NOT_VALID_ACTION(TWI_ACT_ACK);		\
	TWCR = TWCR_ACK;				\
} while(0)

#define TWI_DO_STOP() do {				\
	BUG_IF_NOT_VALID_ACTION(TWI_ACT_ACK);		\
	TWCR = TWCR_ACK;				\
} while(0)

#define TWI_DO_SEND_DATA(data) do {			\
	BUG_IF_NOT_VALID_ACTION(TWI_ACT_SEND_DATA);	\
	TWDR = data;					\
	TWCR = TWCR_BASE; /* XXX: correct? */		\
} while(0)

/* @priv: the arg passed to i2c_xfer().
 * @status: 0 if the transaction was completed, and an error code otherwise
 *         (FIXME: define error codes)
 *
 * Callback is expected to interact with TWI hw via macros (ie: do not directly
 * write to TWCR or TWDR).
 *
 * Called with TWI isr disabled (as we are in it) but other interrupts
 * (potentially) enabled.
 */
typedef void (*i2c_master_byte_cb)(void *priv, uint8_t status);

/* @addr is parsed to determine whether this xfer is a read or a write.
 * @cb is stored and later called when more data is needed or an error occurs.
 * @arg is passed to @cb whenever it is called.
 */
void twi_byte_xfer(uint8_t addr, i2c_master_byte_cb cb, void *arg);

bool twi_is_xfer_pending(void);

void twi_init_master(void);

#endif
