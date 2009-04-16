#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "queue.h"
#include "usart1.h"
#include "usart1_conf.h"

QUEUE_BASE_T _tx_buffer[QUEUE_SZ];
queue_t tx_q;

static int usart1_putchar_queue_ts3(char c, FILE *stream);
static FILE usart1_io_queue = FDEV_SETUP_STREAM(usart1_putchar_queue_ts3, NULL ,_FDEV_SETUP_WRITE);

static int usart1_putchar_direct(char c, FILE *stream);
static FILE usart1_io_direct = FDEV_SETUP_STREAM(usart1_putchar_direct, NULL,_FDEV_SETUP_WRITE);

inline static void usart_udre_inter_on(void)  { UCSRB |= (uint8_t) (1<<UDRIE); }
inline static void usart_udre_inter_off(void) { UCSRB &= (uint8_t)~(1<<UDRIE); }


static int usart1_putchar_direct(char c, FILE *stream) {
	if (c == '\n')
		putc('\r', stream);
	debug_led_on;
	loop_until_bit_is_set(UCSRA, UDRE);
	debug_led_off;
	UDR = c;
	return 0;
}

static int usart1_putchar_queue_ts3(char c, FILE *stream) {		if (c == '\n')
		putc('\r', stream);

	uint8_t sreg = SREG;
	cli();
	if ( q_full(&tx_q) ) {
		usart_udre_inter_on();
		while ( q_full(&tx_q) ) { 
			sei();
			asm(
				"nop" "\n\t"
				"nop" "\n\t"
			);
			cli();
		}
		q_push(&tx_q,c);
		SREG = sreg;
		return 0;
	}	
	q_push(&tx_q,c);
	usart_udre_inter_on();
	SREG = sreg;
	return 0;
}



void usart1_init(void) {
	power_usart_enable();

	q_init(&tx_q, _tx_buffer, QUEUE_SZ);
	/* Set baud rate (12bit) */
	UBRR   = UBRR_VALUE;
	#if USE_2X
	UCSRA |=  (1 << U2X);
	#else
	UCSRA &= (uint8_t) ~(1 << U2X);
	#endif
	// Set frame format: 8data, 1stop bit
	UCSRC = (1<<UCSZ0)|(1<<UCSZ1);
	
	// Enable receiver and transmitter
	UCSRB = 0;	
	UCSRB|= (1<<TXEN); // output from uc
	//UCSRB|= (1<<RXEN); // input to uc

	
	io_queue = &usart1_io_queue;
	io_direct = &usart1_io_direct;
	stdout = io_queue;
	stderr = io_queue;
	io_isr = io_direct;
	io_init = io_direct;
}

ISR(USART_UDRE_vect) {	
	if (tx_q.ct>0) // !q_empty(&tx_q)
		UDR = q_pop(&tx_q);
	if (tx_q.ct<=0)// q_empty(&tx_q)
		usart_udre_inter_off();
}

