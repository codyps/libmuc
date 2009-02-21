#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "queue.h"
#include "usart1.h"

static int usart1_putchar_queue_ts1(char c, FILE *stream);
static int usart1_putchar_queue_ts2(char c, FILE *stream);
static int usart1_putchar_queue_ts3(char c, FILE *stream);

static int usart1_putchar_queue(char c, FILE *stream);
static FILE usart1_io_queue = FDEV_SETUP_STREAM(usart1_putchar_queue_ts2, NULL ,_FDEV_SETUP_WRITE);

static int usart1_putchar_direct(char c, FILE *stream);
static FILE usart1_io_direct = FDEV_SETUP_STREAM(usart1_putchar_direct, NULL,_FDEV_SETUP_WRITE);


static void enable_usart1_udre_inter(void)  { UCSR1B|=(1<<UDRIE1); }
static void disable_usart1_udre_inter(void) { UCSR1B&=(uint8_t)~(1<<UDRIE1); }


static int usart1_putchar_direct(char c, FILE *stream) {
        if (c == '\n')
                usart1_putchar_direct('\r', stream);
        loop_until_bit_is_set(UCSR1A, UDRE1);
        UDR1 = c;
        return 0;
}

static int usart1_putchar_queue(char c, FILE *stream) {	
// Unsafe in isr or with other isr's modding the queue.	if (c == '\n')
		putc('\r', stream);

	if (q_full(&tx_q)) {
		PORTB&=(uint8_t)~(1<<6);
		while (q_full(&tx_q));		
	}	
	PORTB|=(1<<6);

	//XXX: do not call from isr!
	disable_usart1_udre_inter();
	q_push(&tx_q,c);
	enable_usart1_udre_inter();

	return 0;
}

static int usart1_putchar_queue_ts1(char c, FILE *stream) {	
// Dies on full queue. Safe in all cases, but not reliable.	if (c == '\n')
		putc('\r', stream);

	uint8_t sreg = SREG;
	cli();
	{
		if (q_full(&tx_q)) {
			PORTB&=(uint8_t)~(1<<6);		
			SREG=sreg;
			return -1;
		}	
		PORTB|=(1<<6);
		q_push(&tx_q,c);
		enable_usart1_udre_inter();
	}
	SREG=sreg;
	return 0;
}


static int usart1_putchar_queue_ts2(char c, FILE *stream) {
// If the queue is full, enable interupt and waits for non-full queue.
// Safe only in non isr calls.		if (c == '\n')
		putc('\r', stream);

	uint8_t sreg = SREG;
	cli();
	if (q_full(&tx_q)) {
		PORTB&=(uint8_t)~(1<<6);
		enable_usart1_udre_inter();
		while (q_full(&tx_q)) { 
			sei();
			asm(	
				"nop" "\n\t"
				"nop" "\n\t"	);
			cli();
		}
	}

	PORTB|=(1<<6);
	q_push(&tx_q,c);
	SREG=sreg;
	return 0;
}

static int usart1_putchar_queue_ts3(char c, FILE *stream) {		if (c == '\n')
		putc('\r', stream);

	uint8_t sreg = SREG;
	cli();
	if (q_full(&tx_q)) {
		PORTB&=(uint8_t)~(1<<6);
		enable_usart1_udre_inter();
		while ( q_full(&tx_q) ) { 
			sei();
			asm(
				"nop" "\n\t"
				"nop" "\n\t"
			);
			cli();
		}
	}	
	
	PORTB|=(1<<6);
	q_push(&tx_q,c);
	enable_usart1_udre_inter();
	SREG=sreg;
	return 0;
}

static int usart1_putchar_queue_ts4a(char c, FILE *stream) {

	if (c == '\n')
		putc('\r', stream);

	uint8_t sreg = SREG;
	cli();
	while (q_push(&tx_q)) {
		PORTB&=(uint8_t)~(1<<6);
		return -1;
	}

	PORTB|=(1<<6);

	enable_usart1_udre_inter();
	SREG=sreg;
	return 0;
}

static int usart1_putchar_queue_ts4b(char c, FILE *stream) {	
	if (c == '\n')
		putc('\r', stream);

	uint8_t sreg = SREG;
	cli();
	if (q_push(&tx_q)) {
		enable_usart1_udre_inter();
		while (q_push(&tx_q)) {
			sei();
			PORTB&=(uint8_t)~(1<<6);
			asm("nop");
			cli();
		}
		PORTB|=(1<<6);
		SREG=sreg;
		return 0;
	}
	enable_usart1_udre_inter();
	SREG=sreg;
	return 0;
}


void usart1_init(void) {
	power_usart1_enable();

	q_init(&tx_q, _tx_buffer, QUEUE_SZ);
	/* Set baud rate (12bit) */
	UBRR1   = UBRR_VALUE;
	#if USE_2X
	UCSR1A |=  (1 << U2X1);
	#else
	UCSR1A &= (uint8_t) ~(1 << U2X1);
	#endif
	// Set frame format: 8data, 1stop bit
	UCSR1C = (1<<UCSZ10)|(1<<UCSZ11);
	
	// Enable receiver and transmitter
	UCSR1B = 0;	
	UCSR1B|= (1<<TXEN1); // output from uc
	//UCSR1B|= (1<<RXEN1); // input to uc

	stderr=stdout=&usart1_io_queue;
	io_direct = &usart1_io_direct;
	io_isr = io_direct;
	io_init = io_direct;
}

ISR(USART1_UDRE_vect) {	
	if (tx_q.ct>0) // !q_empty(&tx_q)
		UDR1 = q_pop(&tx_q);
	if (tx_q.ct<=0)// q_empty(&tx_q)
		disable_usart1_udre_inter();
}

