/* TWI Library
 *
 * Concepts
 *	- a "transaction" is a sequence of i2c "messages" delimited by repeated
 *	  starts (XXX: currently the case)
 *	- a "message" is an address (potentially with the write bit set) followed
 *	  by a series of data bytes that is terminated by a repeated start or a
 *	  stop.
 *
 * Design
 *  A user of this api assembles a stream of bytes representing a transaction,
 *  then passes this byte stream plus a callback function (and optional
 *  argument for the callback) to twi_xfer().
 *
 *  Allocation of the byte stream is the responsibility of the caller of
 *  twi_xfer(). The transaction buffer should be allocated by using the
 *  TWI_SZ_* macros to determine it's sizing.
 *
 *  When the transaction completes or encounters an error, the callback is
 *  invoked with a pointer to the transactions buffer, the callback argument,
 *  and a status parameter.
 *
 * Encoding Transactions
 *
 *   There are 2 types of messages which may be encoded: indirect or embedded.
 *
 *   Indirect messages store an i2c address, a length and a pointer into the
 *   transaction. The pointer points to a memory region to be read from or
 *   written to. The size of the memory region is indicated by the stored length.
 *
 *   Embedded messages store an i2c address, a length, and a number of data
 *   bytes in the transaction. While indirect messages use external (pointed
 *   to) regions of memory for input and output, embedded messages use the
 *   transaction itself as their data store.
 */

/* XXX: should the api operate per message & have no knowledge of transactions?
 *
 * In that case, the callback would be able to (for example) issue a repstart
 * and a new message xfer to have the same functionality as transactions.
 */

/* XXX: will we ever want to delimit messages in a transaction with STOPs?
 *
 * The callback-per-message api would allow this to be solved easily, the
 * callback-per-transaction api requires additional encoding in the transaction
 * byte stream  to support this. */

/* XXX: should the api operate with per byte callbacks?
 *
 * This would allow the data to be directly interpereted without the need for a
 * recving buffer. */

/* XXX: callback-per-transaction api currently is designed with the message
 * length limitted to 255 bytes.
 */

#ifndef TWI2_H_
#define TWI2_H_

#include <stdint.h>
#include <util/twi.h> /* implicitly includes avr/io.h */
#include <penny/penny.h> /* ARRAY_SZ */

struct i2c__indirect_msg {
	uint8_t addr;
	uint8_t len;
	uint8_t *data;
};

struct i2c__embedded_msg {
	uint8_t addr;
	uint8_t marker; /* always has value TWI_MSG_MARKER */
	uint8_t len;
	uint8_t data[];
};

typedef struct i2c_trans {
	uint8_t ct;
	uint8_t __data[];
} i2c_trans;

/* a pointer into a transaction, points to a particular message */
typedef struct i2c_msg {
	uint8_t addr;
	uint8_t __marker_or_len;
	uint8_t __data[];
} i2c_msg;

/* @trans: the transaction that was passed to i2c_xfer().
 * @msg: the last message that was processed (either successful or not, either sent or recived).
 * @priv: the arg passed to i2c_xfer().
 * @status: 0 if the transaction was completed, and an error code otherwise
 *	Error codes: -1:  data write was nacked
 *		     -2:  read addr send was nacked
 *		     -3:  write addr send was nacked
 *		     -4:  arbitration failure
 *
 * Called by twi_service(), which should be placed in the main loop.
 */
typedef void (*i2c_master_cb)(i2c_trans *trans, i2c_msg *msg, int8_t status);

/* TWI_SZ_* : these should be used to size the allocation for a i2c_trans.
 *
 * For example, encoding a single embedded write message which sends 3 bytes:
 *
 *     static char buf[TWI_SZ_BASE + TWI_SZ_EWMSG(3)];
 *     i2c_trans *trans = buf;
 */
#define TWI_SZ_BASE 1 /* count */
/* XXX: doesn't really have spare bits in pointer, ROM can be up to 64k in size */
#define TWI_SZ_IWMSG	     (1 /* addr */ + 1 /* len */ + sizeof(char *))
/* XXX: the pointer has some spare bits, RAM is only max ~2k */
#define TWI_SZ_IRMSG         (1 /* addr */ + 1 /* len */ + sizeof(char *))
#define TWI_SZ_EWMSG(bytes)  (1 /* addr */ + 1 /* marker */ + 1 /* len */ + bytes)
#define TWI_SZ_ERMSG(bufsz)  (1 /* addr */ + 1 /* marker */ + 1 /* len */ + bufsz)

#define TWI_MSG_MARKER 0xff
#define TWI_WMSG_IS_EMBEDDED(msg) ((msg)[1] == TWI_MSG_MARKER)
#define TWI_RMSG_IS_EMBEDDED(msg) ((msg)[1] == TWI_MSG_MARKER)

#define TWI_IRMSG_LEN_IX 1
#define TWI_IWMSG_LEN_IX 1
#define TWI_ERMSG_LEN_IX 2
#define TWI_EWMSG_LEN_IX 2

static inline i2c_msg *twi_trans_first_msg(struct i2c_trans *t)
{
	return (i2c_msg *)&t->__data;
}

/* */
void twi_trans_init(i2c_trans *trans, size_t len);

/* delimit messages in a transaction. */
void twi_trans_start_msg(i2c_trans *trans, uint8_t addr);
/* if a message contains this, this _MUST_ be the only element */
void twi_trans_send_from(i2c_trans *trans, uint8_t *wbuf, uint8_t sz);
/* if a message contains this, only this & repetitions of this may be in that message */
void twi_trans_send(i2c_trans *trans, uint8_t byte);
/* if a message contains this, this _MUST_ be the only element */
void twi_trans_recv_into(i2c_trans *trans, uint8_t *rbuf, uint8_t sz);
/* if a message contains this, this _MUST_ be the only element */
void twi_trans_recv(i2c_trans *trans, uint8_t len);

#define trans_for_each_msg(trans, msg_ix, curr_msg)	\
		for(curr_msg = twi_trans_first_msg(trans), msg_ix = 0;	\
		    msg_ix < ((i2c_trans *)trans)->ct;	\
		    curr_msg = trans_next_msg(curr_msg)	\
		    , msg_ix ++)

void twi_xfer(i2c_trans *trans, i2c_master_cb cb, void *arg);
bool twi_is_xfer_pending(void);
void twi_init_master(void);

/** slave **/
struct i2c_slave;
typedef void    (*i2c_slave_recv_cb)(uint8_t addr, uint8_t data, struct i2c_slave *s);
typedef uint8_t (*i2c_slave_send_cb)(uint8_t addr, struct i2c_slave *s);

struct i2c_slave {
	uint8_t addr;
	uint8_t mask; /* Possibly: replace addr&mask with another matcher */
	i2c_slave_send_cb s;
	i2c_slave_recv_cb r;
};

/* @s is expected to stay allocated & is not copied.
 * It is expected that @s will point to a member of a slave-specific data
 * structure, & the original pointer will be retrieved with container_of. */
void twi_init_slave(struct i2c_slave *s);

#endif
