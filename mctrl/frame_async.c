#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "common.h"
#include "ds/circ_buf.h"

#include "proto.h"

/* 0x7f => 0x7d, 0x5f
 * 0x7e => 0x7d, 0x5e
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

#define sizeof_member(type, member) \
	sizeof(((type *)0)->member)

struct packet_buf {
	uint8_t buf[32]; /* bytes */

	/* array of packet starts in bytes (byte heads and tails,
	 * depending on index */
	uint8_t p_idx[8];
	uint8_t head; /* next packet_idx_buf loc to read from (head_packet) */
	uint8_t tail; /* next packet_idx_buf loc to write to  (tail_packet) */
};

static struct packet_buf rx, tx;


#if defined(AVR)
/* {{ DEBUG */
# ifdef DEBUG
static int usart0_putchar_direct(char c, FILE *stream) {
	if (c == '\n')
		putc('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

static void print_wait(void)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
}

static FILE usart0_io_direct =
	FDEV_SETUP_STREAM(usart0_putchar_direct, NULL,_FDEV_SETUP_WRITE);
# endif

# define usart0_udre_isr_on() (UCSR0B |= (1 << UDRIE0))
# define usart0_udre_unlock() do {         \
		UCSR0B |= (1 << UDRIE0);  \
		asm("":::"memory");       \
	} while(0)

# define usart0_udre_isr_off() (UCSR0B &= ~(1 << UDRIE0))
# define usart0_udre_lock() do {           \
		UCSR0B &= ~(1 << UDRIE0); \
		asm("":::"memory");       \
	} while(0)

# define RX_BYTE_GET() UDR0
# define RX_STATUS_GET() UCSR0A
# define RX_STATUS_IS_ERROR(status) ((status) & ((1 << FE0) | (1 << DOR0) | (1<< UPE0)))

# define TX_BYTE_SEND(byte) (UDR0 = (byte))

# define RX_ISR() ISR(USART_RX_vect)
# define TX_ISR() ISR(USART_UDRE_vect)

#else /* !defined(AVR) */

# ifdef DEBUG
#  define print_wait()
# endif

# define usart0_udre_isr_on()
# define usart0_udre_unlock()
# define usart0_udre_isr_off()
# define usart0_udre_lock()

# define RX_BYTE_GET() getchar()
# define RX_STATUS_GET() 0
# define RX_STATUS_IS_ERROR(status) false

# define TX_BYTE_SEND(byte) putchar(byte)

# define TX_ISR() void frame_tx_isr(void)
# define RX_ISR() void frame_rx_isr(void)

#endif


#ifdef DEBUG
static void print_packet_buf(struct packet_buf *b)
{
	printf("head %02d  tail %02d  p_idx(%d) ", b->head, b->tail, sizeof(b->p_idx));
	uint8_t i;
	for (i = 0; ;) {
		printf("%d", b->p_idx[i]);
		i++;
		if (i < sizeof(b->p_idx))
			putchar(' ');
		else
			break;
	}

	printf("  buf ");
	for(i = 0; i < sizeof(b->buf); i++) {
		printf("%c ", b->buf[i]);
	}
}

void frame_timeout(void)
{
	printf("\n{{ tx: ");
	print_packet_buf(&tx);
	printf(" }}\n{{ rx: ");
	print_packet_buf(&rx);
	printf(" }}\n");

}
#endif
/* DEBUG }} */

/*** Reception of Data ***/
/** receive: consumer, modifies tail **/
/* called first to determine if we have a packet */
uint8_t frame_recv_len(void)
{
	uint8_t p_tail = rx.tail;
	if (p_tail != rx.head) {
		return CIRC_CNT(rx.p_idx[CIRC_NEXT(p_tail,sizeof(rx.p_idx))],
				rx.p_idx[p_tail],
				sizeof(rx.buf));
	} else {
		return 0;
	}
}

/* get next byte from packet */
uint8_t frame_recv_byte(void)
{
	uint8_t p_curr_tail = rx.tail;
	uint8_t p_next_tail = CIRC_NEXT(p_curr_tail, sizeof(rx.p_idx));
	uint8_t b_curr_tail = rx.p_idx[p_curr_tail];
	uint8_t b_next_tail = rx.p_idx[p_next_tail];

	if (b_curr_tail != b_next_tail) {
		uint8_t data = rx.buf[b_curr_tail];
		rx.p_idx[p_curr_tail] = CIRC_NEXT(b_curr_tail, sizeof(rx.buf));
		return data;
	} else {
		return 0;
	}
}

/* returns length of packet */
uint8_t frame_recv_copy(uint8_t *dst, uint8_t len)
{
	uint8_t curr_tail = rx.tail;
	if (curr_tail != rx.head) {
		uint8_t curr_b_tail = rx.p_idx[curr_tail];
		uint8_t next_tail = CIRC_NEXT(rx.tail, sizeof(rx.p_idx));
		uint8_t next_b_tail = rx.p_idx[next_tail];
		uint8_t ct = CIRC_CNT(next_b_tail, curr_b_tail, sizeof(rx.buf));
		uint8_t ct_to_end = CIRC_CNT_TO_END(next_b_tail,
					curr_b_tail, sizeof(rx.buf));

		uint8_t cpy1_len = MIN(len, ct_to_end);
		uint8_t cpy2_len = MIN(len, ct) - cpy1_len;

		memcpy(dst, rx.buf + curr_b_tail, cpy1_len);
		memcpy(dst + cpy1_len, rx.buf, cpy2_len);

		return ct;
	} else {
		return 0;
	}
}

void frame_recv_next(void)
{
	/* This should only be called following frame_recv_len succeeding */
	rx.tail = CIRC_NEXT(rx.tail,sizeof(rx.p_idx));
}

uint8_t frame_recv_ct(void)
{
	return CIRC_CNT(rx.head,
			rx.tail,
			sizeof(rx.p_idx));
}

/** recieve: producer, modifies head **/
RX_ISR()
{
	static bool is_escaped;
	static bool recv_started;
	uint8_t status = RX_STATUS_GET();
	uint8_t data = RX_BYTE_GET();

	/* safe location (in rx.p_idx) to store the location of the next
	 * byte to write; */
	uint8_t next_head = CIRC_NEXT(rx.head,sizeof(rx.p_idx));

	/* check `status` for error conditions */
	if (RX_STATUS_IS_ERROR(status)) {
		/* frame error, data over run, parity error */
		goto drop_packet;
	}

	if (data == START_BYTE) {
		/* prepare for start, reset packet position, etc. */
		/* packet length is non-zero */
		recv_started = true;
		is_escaped = false;

		/* is there any data in the packet? */
		if (rx.p_idx[rx.head] != rx.p_idx[next_head]) {
			/* Need to get some data for packet to be valid */
			uint8_t next_next_head = CIRC_NEXT(next_head, sizeof(rx.p_idx));
			if (next_next_head == rx.tail) {
				/* no space in p_idx for another packet */

				/* Essentailly a packet drop, but we want
				 * recv_started set as this is a START_BYTE,
				 * after all. */
				rx.p_idx[next_head] = rx.p_idx[rx.head];
			} else {
				/* advance the packet idx */
				rx.head = next_head;

				/* rx.p_idx[next_head] will be set correctly,
				 * update rx.p_idx[next_next_head] to be the
				 * same as rx.p_idx[next_head]
				 */
				rx.p_idx[next_next_head] = rx.p_idx[next_head];
			}
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

	/* do we have another byte to write into? */
	/* FIXME: don't need full CIRC_SPACE calculation */
	if (CIRC_SPACE(rx.p_idx[next_head], rx.p_idx[rx.tail], sizeof(rx.buf))) {
		rx.buf[rx.p_idx[next_head]] = data;
		rx.p_idx[next_head] =
			(rx.p_idx[next_head] + 1) & (sizeof(rx.buf) - 1);

		return;
	}


	/* well, shucks. we're out of space, drop the packet */
	/* goto drop_packet; */

drop_packet:
	recv_started = false;
	is_escaped = false;
	/* first byte of the sequence we are writing to; */
	rx.p_idx[next_head] = rx.p_idx[rx.head];
}

/*** Transmision of Data ***/
/** transmit: consumer of data, modifies tail **/
TX_ISR()
{
	/* Only enabled when we have data.
	 * Bytes inseted into location indicated by next_tail.
	 * Tail advanced on packet completion.
	 *
	 * Expectations:
	 *    tx.p_idx[tx.tail] is the next byte to transmit
	 *    tx.p_idx[next_tail] is the end of the currently
	 *        transmitted packet
	 */
	static bool packet_started;
	uint8_t cur_b_tail = tx.p_idx[tx.tail];
	uint8_t next_tail = CIRC_NEXT(tx.tail,sizeof(tx.p_idx));

	if (cur_b_tail == tx.p_idx[next_tail]) {
		/* no more bytes, signal packet completion */
		/* advance the packet idx. */
		tx.tail = next_tail;
		if (tx.tail == tx.head) {
			usart0_udre_isr_off();
			packet_started = false;
		} else {
			packet_started = true;
		}

		TX_BYTE_SEND(START_BYTE);
		return;
	}

	/* Error case for UDRIE enabled when ring empty
	 * no packet indexes seen */
	if (tx.tail == tx.head) {
		packet_started = false;
		usart0_udre_isr_off();
		return;
	}

	/* is it a new packet? */
	if (!packet_started) {
		packet_started = true;
		TX_BYTE_SEND(START_BYTE);
		return;
	}

	uint8_t data = tx.buf[cur_b_tail];

	if (data == START_BYTE || data == ESC_BYTE || data == RESET_BYTE) {
		TX_BYTE_SEND(ESC_BYTE);
		tx.buf[cur_b_tail] = data ^ ESC_MASK;
		return;
	}

	TX_BYTE_SEND(data);

	/* Advance byte pointer */
	tx.p_idx[tx.tail] = CIRC_NEXT(cur_b_tail,sizeof(tx.buf));
}

/** transmit: producer of data, modifies head **/
static bool frame_start_flag;
void frame_start(void)
{
	if (CIRC_SPACE(tx.head, tx.tail, sizeof(tx.p_idx))) {
		frame_start_flag = true;
		tx.p_idx[CIRC_NEXT(tx.head,sizeof(tx.p_idx))] = tx.p_idx[tx.head];
	}
}

void frame_append_u8(uint8_t x)
{
	if (!frame_start_flag)
		return;

	uint8_t next_head = (tx.head + 1) & (sizeof(tx.p_idx) - 1);
	uint8_t next_b_head = tx.p_idx[next_head];

	/* Can we advance our packet bytes? if not, drop packet */
	if (CIRC_SPACE(next_b_head, tx.p_idx[tx.tail], sizeof(tx.buf)) < 1) {
		tx.p_idx[next_head] = tx.p_idx[tx.head];
		frame_start_flag = false;
	}

	tx.buf[next_b_head] = x;
	CIRC_NEXT_EQ(tx.p_idx[next_head], sizeof(tx.buf));
}

void frame_append_u16(uint16_t x)
{
	if (!frame_start_flag)
		return;

	uint8_t next_head = (tx.head + 1) & (sizeof(tx.p_idx) - 1);
	uint8_t next_b_head = tx.p_idx[next_head];

	/* Can we advance our packet bytes? if not, drop packet */
	if (CIRC_SPACE(next_b_head, tx.p_idx[tx.tail], sizeof(tx.buf)) < 2) {
		tx.p_idx[next_head] = tx.p_idx[tx.head];
		frame_start_flag = false;
	}

	tx.buf[next_b_head] = (uint8_t)(x >> 8);
	tx.buf[(next_b_head + 1) & (sizeof(tx.buf) - 1)] = (uint8_t)x & 0xFF;

	tx.p_idx[next_head] = (tx.p_idx[next_head] + 2) & (sizeof(tx.buf) - 1);
}

void frame_done(void)
{
	if (!frame_start_flag)
		return;

	uint8_t new_head = (tx.head + 1) & (sizeof(tx.p_idx) - 1);
	uint8_t new_next_head = (new_head + 1) & (sizeof(tx.p_idx) - 1);

	/* Set in this ordering to avoid a race (the moved tx.head indicates
	 * imediatly that new data can be read) */
	tx.p_idx[new_next_head] = tx.p_idx[new_head];
	tx.head = new_head;
	usart0_udre_unlock();
	frame_start_flag = false;
}

void frame_send(const void *data, uint8_t nbytes)
{
	uint8_t cur_head = tx.head;
	uint8_t cur_b_head = tx.p_idx[cur_head];
	uint8_t cur_tail = tx.tail;
	uint8_t cur_b_tail= tx.p_idx[cur_tail];

	/* we can fill .buf up completely only in the case that the packet
	 * buffer has more than 1 packet (which is very likely), so use
	 * the standard circ buffer managment here to keep the space open */
	uint8_t space = CIRC_SPACE(cur_b_head, cur_b_tail, sizeof(tx.buf));

	/* Can we advance our packet bytes? if not, drop packet */
	if (nbytes > space) {
		return;
	}

	uint8_t next_head = CIRC_NEXT(cur_head, sizeof(tx.p_idx));
	/* do we have space for the packet_idx? */
	if (next_head == cur_tail) {
		return;
	}

	/* amount to copy in first memcpy */
	uint8_t space_to_end =
		MIN(CIRC_SPACE_TO_END(cur_b_head, cur_b_tail, sizeof(tx.buf)),
				nbytes);

	/* copy first segment of data (may be split) */
	memcpy(tx.buf + cur_b_head, data, space_to_end);

	/* copy second segment if it exsists (nbytes - space_to_end == 0
	 * when it doesn't) */
	memcpy(tx.buf, data + space_to_end, nbytes - space_to_end);

	/* advance packet length */
	tx.p_idx[next_head] = (cur_b_head + nbytes) & (sizeof(tx.buf) - 1);

	/* advance packet idx */
	/* XXX: if we usart0_udre_lock() prior to setting tx.head,
	 * the error check in the ISR for an empty packet can be avoided.
	 * As we know that the currently inserted data will not have been
	 * processed, while without the locking if the added packet is short
	 * enough and the ISR is unlocked when we set tx.head, the ISR may be
	 * able to process the entire added packet and disable itself prior
	 * to us calling usart0_udre_unlock().
	 */
	tx.head = next_head;

	/* XXX: do we need to set
	 * tx.p_idx[next_next_head] = tx.p_idx[next_head]
	 * ? */
	usart0_udre_unlock();
}


/*** Initialization ***/
#ifdef AVR
static void usart0_init(void)
{
	/* Disable ISRs, recv, and trans */
	UCSR0B = 0;

	/* Asyncronous, parity odd, 1 bit stop, 8 bit data */
	UCSR0C = (0 << UMSEL01) | (0 << UMSEL00)
		| (1 << UPM01)  | (1 << UPM00)
		| (0 << USBS0)
		| (1 << UCSZ01) | (1 << UCSZ00);

	/* Baud 38400 */
#define BAUD 38400
#include <util/setbaud.h>
	UBRR0 = UBRR_VALUE;

#if USE_2X
	UCSR0A = (1 << U2X0);
#else
	UCSR0A = 0;
#endif

	/* Enable RX isr, disable UDRE isr, EN recv and trans, 8 bit data */
	UCSR0B = (1 << RXCIE0) | (0 << UDRIE0)
		| (1 << RXEN0) | (1 << TXEN0)
		| (0 << UCSZ02);

	/* XXX: debugging */
#if defined(DEBUG)
	stdout = stderr = &usart0_io_direct;
#endif
}

void frame_init(void)
{
	usart0_init();
}
#endif /* AVR */
