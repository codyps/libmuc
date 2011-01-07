#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "ds/circ_buf.h"
#include "usart.h"
#include "usart_def.h"
#include "common.h"

static struct tq {
	uint8_t head;
	uint8_t tail;
	char buf[64];
} tq;

static struct rq {
	uint8_t head;
	uint8_t tail;
	char buf[USART_RX_BUFF_SZ];
} rq;

static uint8_t usart_msg;

static int usart_getchar(FILE * stream);
static int usart_putchar(char c, FILE *stream);
static FILE usart_io = FDEV_SETUP_STREAM(usart_putchar
		, usart_getchar
		, _FDEV_SETUP_RW);

static inline void usart_udrei_on(void)
{
	UCSRB |= (1 << UDRIE);
}

static inline void usart_udrei_off(void)
{
	UCSRB &= ~(1 << UDRIE);
}

static inline void usart_rxi_on(void)
{
	UCSRB|= (1 << RXCIE);
}

static inline void usart_rxi_off(void)
{
	UCSRB &= ~(1 << RXCIE);
}

void usart_flush_rx(void)
{
	rq.tail = rq.head;
}

void usart_flush_tx(void)
{
	tq.tail = tq.head;
}

void usart_flush_rx_to(char end_delim)
{
	while(rq.tail != rq.head) {
		uint8_t d = rq.buf[rq.tail];
		if (d == end_delim)
			return;
		rq.tail = CIRC_NEXT(rq.tail, sizeof(rq.buf));
	}
}

bool usart_new_msg(void)
{
	bool ret = false;
	usart_rxi_off();
	if (usart_msg) {
		usart_msg--;
		ret = true;
	}
	usart_rxi_on();
	return ret;
}

bool usart_hasc(void)
{
	return rq.tail != rq.head;
}

char usart_getc(void)
{
	char ret = rq.buf[rq.tail];
	rq.tail = CIRC_NEXT(rq.tail, sizeof(rq.buf));
	return ret;
}

static int usart_getchar(FILE *stream)
{
	/* only the head should change, cache the tail */
	uint8_t cur_tail = rq.tail;
	while(cur_tail == rq.head) {
		barrier();
	}

	char ret = rq.buf[cur_tail];
	rq.tail = CIRC_NEXT(cur_tail, sizeof(rq.buf));
	return ret;
}

static int usart_putchar(char c, FILE *stream) {
	/* we only expect the tail to change, not the head. */
	uint8_t next_head = CIRC_NEXT(tq.head,sizeof(tq.buf));
	while(tq.tail == next_head) {
		barrier();
	}

	tq.buf[next_head] = c;
	tq.head = next_head;
	return 0;
}

void usart_init(void) {
	power_usart_enable();
	UCSRB = 0;

	/* Set baud rate (12bit) */
	UBRR   = UBRR_VALUE;
	#if USE_2X
	UCSRA |=  (1 << U2X);
	#else
	UCSRA &= ~(1 << U2X);
	#endif
	// Set frame format: 8data, 1stop bit
	UCSRC = (1<<UCSZ0)|(1<<UCSZ1);

	// Enable receiver and transmitter
	UCSRB |= (1<<TXEN); // output from uc
	UCSRB |= (1<<RXEN); // input to uc

	UCSRB |= (1<<RXCIE); //enable rx isr

	stdout = &usart_io;
	stdin  = &usart_io;
	stderr = &usart_io;
}

ISR(USART_UDRE_vect)
{
	if (tq.head == tq.tail) {
		usart_udrei_off();
		return;
	}

	UDR = tq.buf[tq.tail];
	tq.tail = CIRC_NEXT(tq.tail, sizeof(tq.buf));
}

ISR(USART_RX_vect)
{
	if (rq.tail == CIRC_NEXT(rq.head, sizeof(rq.buf))) {
		return;
	}

	rq.buf[rq.head] = UDR;
	if (rq.buf[rq.head] == '\n')
		usart_msg ++;

	rq.head = CIRC_NEXT(rq.head, sizeof(rq.buf));
}


