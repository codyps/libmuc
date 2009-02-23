#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "queue.h"
#include "usart1.h"

QUEUE_BASE_T _tx_buffer[QUEUE_SZ];
queue_t tx_q;

static int usart1_putchar_queue_ts2(char c, FILE *stream);
static int usart1_putchar_queue_ts3(char c, FILE *stream);

static FILE usart1_io_queue = FDEV_SETUP_STREAM(usart1_putchar_queue_ts3, NULL ,_FDEV_SETUP_WRITE);

static int usart1_putchar_direct(char c, FILE *stream);
static FILE usart1_io_direct = FDEV_SETUP_STREAM(usart1_putchar_direct, NULL,_FDEV_SETUP_WRITE);

static void usart1_udre_inter_on(void)  { UCSR1B|=(1<<UDRIE1); }
static void usart1_udre_inter_off(void) { UCSR1B&=(uint8_t)~(1<<UDRIE1); }


static int usart1_putchar_direct(char c, FILE *stream) {
	if (c == '\n')
		putc('\r', stream);
	PORTB&=(uint8_t)~(1<<6);
	loop_until_bit_is_set(UCSR1A, UDRE1);
	PORTB|=(1<<6);
	UDR1 = c;
	return 0;
}

static int usart1_putchar_queue_ts2(char c, FILE *stream) {
// If the queue is full, enable interupt and waits for non-full queue.
// Safe only in non or protected isr calls.	if (c == '\n')
		putc('\r', stream);

	uint8_t sreg = SREG;
	cli();
	if (q_full(&tx_q)) {
		PORTB&=(uint8_t)~(1<<6);
		usart1_udre_inter_on();
		while (q_full(&tx_q)) { 
			sei();
			asm(	
				"nop" "\n\t"
				"nop" "\n\t"	);
			cli();
		}
		PORTB|=(1<<6);
	}
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
		usart1_udre_inter_on();
		while ( q_full(&tx_q) ) { 
			sei();
			asm(
				"nop" "\n\t"
				"nop" "\n\t"
			);
			cli();
		}
		PORTB|=(1<<6);
		q_push(&tx_q,c);
		SREG=sreg;
		return 0;
	}	
	usart1_udre_inter_off();
	q_push(&tx_q,c);
	usart1_udre_inter_on();
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

	
	io_queue = &usart1_io_queue;
	io_direct = &usart1_io_direct;
	stdout = io_queue;
	stderr = io_queue;
	io_isr = io_direct;
	io_init = io_direct;
}

ISR(USART1_UDRE_vect) {	
	if (tx_q.ct>0) // !q_empty(&tx_q)
		UDR1 = q_pop(&tx_q);
	if (tx_q.ct<=0)// q_empty(&tx_q)
		usart1_udre_inter_off();
}

