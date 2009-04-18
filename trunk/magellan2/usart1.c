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

static int usart1_getchar_direct_2(FILE * stream);
static int usart1_getchar_direct(FILE * stream);
static int usart1_putchar_direct(char c, FILE *stream);
static FILE usart1_io_direct = FDEV_SETUP_STREAM(usart1_putchar_direct, usart1_getchar_direct_2,_FDEV_SETUP_RW);



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

static int usart1_getchar_direct(FILE * stream) {
	 loop_until_bit_is_set(UCSRA, RXC);
	char c = UDR;
	return c;
}

#define RX_BUFSIZE 64
static int usart1_getchar_direct_2(FILE *stream) {
	uint8_t c;
	char *cp, *cp2;
	static char b[RX_BUFSIZE];
	static char *rxp;
	
	if (rxp == 0) {
		for (cp = b;;) {
			loop_until_bit_is_set(UCSR0A, RXC0);
			if (UCSR0A & _BV(FE0))	return _FDEV_EOF;
			if (UCSR0A & _BV(DOR0))	return _FDEV_ERR;
			c = UDR0;
			// behaviour similar to Unix stty ICRNL
			if (c == '\r') c = '\n';
			if (c == '\n') {
				*cp = c;
				fputc(c, stream);
				rxp = b;
				break;
			}
			else if (c == '\t') 	c = ' ';
			
			if ((c >= (uint8_t)' ' && c <= (uint8_t)'\x7e') || c >= (uint8_t)'\xa0') {
				if (cp == b + RX_BUFSIZE - 1)
					fputc('\a', stream);
				else {
					*cp++ = c;
					fputc(c, stream);
				}
				continue;
			}
				
			switch (c) {
			  case 'c' & 0x1f:
				return -1;
				
			  case '\b':
			  case '\x7f':
				if (cp > b) {
					fputc('\b', stream);
					fputc(' ', stream);
					fputc('\b', stream);
					cp--;
				}
				break;
				
			  case 'r' & 0x1f:
				fputc('\r', stream);
				for (cp2 = b; cp2 < cp; cp2++)
					fputc(*cp2, stream);
				break;
				
			  case 'u' & 0x1f:
				while (cp > b) {
					fputc('\b', stream);
					fputc(' ', stream);
					fputc('\b', stream);
					cp--;
				}
				break;
				
			  case 'w' & 0x1f:
				while (cp > b && cp[-1] != ' ') {
					fputc('\b', stream);
					fputc(' ', stream);
					fputc('\b', stream);
					cp--;
				}
				break;
			}
		}
	}
	c = *rxp++;
	if (c == '\n')	rxp = 0;
	return c;
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

