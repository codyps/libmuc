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

QUEUE_BASE_T _rx_buffer[QUEUE_SZ];
queue_t rx_q;

static int usart1_getchar_queue(FILE * stream);
static int usart1_putchar_queue_ts3(char c, FILE *stream);
static FILE usart1_io_queue = FDEV_SETUP_STREAM(usart1_putchar_queue_ts3, usart1_getchar_queue ,_FDEV_SETUP_RW);

static int usart1_getchar_direct(FILE * stream);
static int usart1_putchar_direct(char c, FILE *stream);
static FILE usart1_io_direct = FDEV_SETUP_STREAM(usart1_putchar_direct, usart1_getchar_direct,_FDEV_SETUP_RW);

inline static void usart_udre_inter_on(void)  { UCSRB |= (uint8_t) (1<<UDRIE); }
inline static void usart_udre_inter_off(void) { UCSRB &= (uint8_t)~(1<<UDRIE); }

queue_t * _get_queue(uint8_t i) {
	if (i == 0)
		return &tx_q;
	else if (i == 1)
		return &rx_q;
	else return NULL;
}

void usart1_flush_rx( void ) {
    q_flush(&rx_q);
}

void usart1_flush_tx( void ) {
    q_flush(&tx_q);
}

static int usart1_putchar_direct(char c, FILE *stream) {
	if (c == '\n')
		putc('\r', stream);
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
	return 0;
}

static int usart1_getchar_queue(FILE *stream) {
	if ( !q_empty(&rx_q) )
		return q_pop(&rx_q);
	else
		return _FDEV_EOF;
}

static int usart1_putchar_queue_ts3(char c, FILE *stream) {	if (c == '\n')
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
	q_init(&rx_q, _rx_buffer, QUEUE_SZ);
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
	UCSRB|= (1<<RXEN); // input to uc

	UCSRB|= (1<<RXCIE); //enable rx isr

	io_queue = &usart1_io_queue;
	io_direct = &usart1_io_direct;
	stdout = io_queue;
	stdin  = io_queue;
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

ISR(USART_RX_vect) {
	char c;
	char ucsra;

	ucsra = UCSRA;

	c = UDR;

	if ( !q_full(&rx_q) && !( ucsra &(1<<FE1) || ucsra & (1<<DOR1) ) ) {

		// Characters that require response.
		if ( c == '\b' || c == 0x7f )	{	// backspace
			q_remove(&rx_q);
			fputc('\b',&usart1_io_queue);
			fputc(' ' ,&usart1_io_queue);
			fputc('\b',&usart1_io_queue);
		}

        	if ( c == '\r') c = '\n';

		if ( c == '\n' ) {
			usart_msg++;
            // copy to some type of message buffer?
		}

        	q_push(&rx_q,c);
		fputc(c,&usart1_io_queue);

	}
	else {
	    // Return 'alarm' characters when the queue is full / error occoured.
		fputc('\a', &usart1_io_queue);
	}
}



#define RX_BUFSIZE 32
static int usart1_getchar_direct(FILE *stream) {
	uint8_t c;
	char *cp, *cp2;
	static char b[RX_BUFSIZE];
	static char *rxp;

	if (rxp == 0) {
		for (cp = b;;) {
			loop_until_bit_is_set(UCSRA, RXC);
			if (UCSRA & (1<<FE1) )	return _FDEV_EOF;
			if (UCSRA & (1<<DOR1))	return _FDEV_ERR;
			c = UDR;
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
					*cp = c;
					cp ++;
					fputc(c, stream);
				}
				continue;
			}

			switch (c) {
			  case 'c' & 0x1f:
				return -1;

			  case '\b':
			  case '\x7f':	// backspace
				if (cp > b) {
					fputc('\b', stream);
					fputc(' ', stream);
					fputc('\b', stream);
					cp--;
				}
				break;

			  case 'r' & 0x1f:	//??
				fputc('\r', stream);
				for (cp2 = b; cp2 < cp; cp2++)
					fputc(*cp2, stream);
				break;

			  case 'u' & 0x1f:	//??
				while (cp > b) {
					fputc('\b', stream);
					fputc(' ', stream);
					fputc('\b', stream);
					cp--;
				}
				break;

			  case 'w' & 0x1f:	//??
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
