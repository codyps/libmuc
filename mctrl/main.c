#include <avr/io.h>
#include <common/ds/circ_buf.h>

#include "proto.h"

/* 0x7e => 0x7d, 0x5e
 * 0x7d => 0x7d, 0x5d
 */

/* HDLC
 * [flag] [address] [ctrl] [data] [crc] [flag]
 * addr8 : [multicast:1] [node addr:6] ['1':1]
 * addr16: [multicast:1] [node addr:6] ['0':1] [node addr:7] [end addr:1]...
 * ctrl  :
 */

/* ctrl:
 * Frame 7 6 5 4   3 2 1 0
 * I     N(R)  p   N(S)  0
 * S     N(R)  p/f S S 0 1
 * U     M M M p/f M M 1 1
 */

/* LC:
 *  [flag:8] [addr:8] [len:8] [data1:len] [crc:16] [flag:8] ...
 * data1:
 *  []
 */

#define sizeof_member(thing, member) \
	sizeof(((thing *)0)->member)

#define PB_BB_LEN ((uint8_t)128)
#define PB_PI_LEN ((uint8_t)8)
struct packet_buf {
	uint8_t byte_buf[PB_BB_LEN]; /* bytes */
	uint8_t p_idx[PB_PI_LEN]; /* arrays of packet starts in bytes */
	uint8_t head; /* next packet_idx_buf loc to read from */
	uint8_t tail; /* next packet_idx_buf loc to write to  */
};

static struct packet_buf tx, rx;

ISR(USART0_RX_vect)
{
	static bool is_escaped;
	static bool recv_started;
	uint8_t status = UCSR0A;
	uint8_t data = UDR0;

	/* safe location (in rx.p_idx) to store the location of the next
	 * byte to write; */
	uint8_t next_tail = CIRC_NEXT(rx.tail,PB_PI_LEN);

	/* check `status` for error conditions */
	if (status & ((1 << FE0) | (1 << DOR0) | (1<< UPE0))) {
		/* frame error, data over run, parity error */
		goto drop_packet;
	}

	if (data == START_BYTE) {
		/* TODO: prepare for start, reset packet position, etc. */

		if (rx.p_idx[rx.tail] != rx.p_idx[next_tail]) {
			/* packet length is non-zero */
			if (next_tail == CIRC_NEXT(next_tail,PB_PI_LEN)) {
				/* no space in p_idx for another packet */
				return;
			}

			recv_started = true;
			rx.tail = next_tail;

			/* Initial posisition of the next byte is at the
			 * start of the packet */
			rx.p_idx[CIRC_NEXT(rx.tail,PB_PI_LEN)] =
				rx.p_idx[rx.tail];
		}
		/* otherwise, we have zero bytes in the packet, no need to
		 * advance */
		return;
	}

	if (!recv_started) {
		/* ignore stuff until we get a start byte */
		return;
	}

	if (data == RESET_BYTE) {
		/* packet reset */
		goto drop_packet;
	}

	if (data == ESC_BYTE) {
		/* Possible error check: is_escaped should not already
		 * be true */
		is_escaped = true;
		return;
	}

	if (is_escaped) {
		/* Possible error check: is data of one of the allowed
		 * escaped bytes? */
		/* we previously recieved an escape char, transform data */
		is_escaped = false;
		data ^= ESC_MASK;
	}

	/* first byte we can't overwrite; */
	if (rx.p_idx[next_tail] != rx.p_idx[rx.head]) {
		rx.buf[rx.p_idx[next_tail]] = data;
		CIRC_NEXT_EQ(rx.p_idx[next_tail],PB_PB_LEN);
		return;
	} else {
		/* well, shucks. we're out of space, drop the packet */
		goto drop_packet;
	}

drop_packet:
	/* first byte of the sequence we are writing to; */
	rx.p_idx[next_tail] = rx.p_idx[rx.tail];
	recv_started = false;
}

ISR(USART0_TX_vect)
{
	static bool is_escaped;
	/* Check the next byte in the queue */
	uint8_t data = GET();

	if (is_escaped) {
		is_escaped = false;
		UDR0 = data  ^ ESC_MASK;
		return;
	}

	if (data == ESC_BYTE || data == START_BYTE || data == ABORT_BYTE) {
		is_escaped = true;
		UDR0 = ESC_BYTE;
		return;
	}

	UDR0 = data;
}

static void usart0_init(void)
{
	/* Asyncronous 38400 */

	/* 1 start bit
	 * 8 data bits
	 * odd parity
	 * 1 stop bit
	 */
}

static void hldc0_init(void)
{
	usart0_init();
}

__attribute__((noreturn))
void main(void)
{
	hldc0_init(void);
	for(;;) {

	}
}
