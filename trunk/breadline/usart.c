#include <stdio.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "queue.h"
#include "defines.h"
#include "usart.h"


static int usart0_putchar_direct(char c, FILE *stream);
static int usart0_putchar(char c, FILE *stream);
static FILE usart0_stdout = FDEV_SETUP_STREAM(usart0_putchar, NULL,_FDEV_SETUP_WRITE);
static FILE usart0_stderr = FDEV_SETUP_STREAM(usart0_putchar_direct, NULL,_FDEV_SETUP_WRITE);

/*
static int usart_getchar(char c, FILE *stream) {
	return 1;
}
*/

static void enable_usart0_tx_inter(void) {
	UCSR0B|=(1<<UDRIE0);
}
static void disable_usart0_tx_inter(void) {
	UCSR0B&=(uint8_t)~(1<<UDRIE0);
}

static int usart0_putchar_direct(char c, FILE *stream) {
	if (c == '\n')
		usart0_putchar_direct('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

static int usart0_putchar(char c, FILE *stream) {		if (c == '\n')
		usart0_putchar('\r', stream);

	while (q_full(&tx_q));
	disable_usart0_tx_inter();
	q_push(&tx_q,c);	
	enable_usart0_tx_inter();

	return 0;
}
/*
int usart0_getchar_direct(FILE *stream) {
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
				usart0_putchar(c, stream);
				rxp = b;
				break;
			}
			else if (c == '\t') 	c = ' ';
			
			if ((c >= (uint8_t)' ' && c <= (uint8_t)'\x7e') || c >= (uint8_t)'\xa0') {
				if (cp == b + RX_BUFSIZE - 1)
					usart0_putchar('\a', stream);
				else {
					*cp++ = c;
					usart0_putchar(c, stream);
				}
				continue;
			}
				
			switch (c) {
			  case 'c' & 0x1f:
				return -1;
				
			  case '\b':
			  case '\x7f':
				if (cp > b) {
					usart0_putchar('\b', stream);
					usart0_putchar(' ', stream);
					usart0_putchar('\b', stream);
					cp--;
				}
				break;
				
			  case 'r' & 0x1f:
				usart0_putchar('\r', stream);
				for (cp2 = b; cp2 < cp; cp2++)
					usart0_putchar(*cp2, stream);
				break;
				
			  case 'u' & 0x1f:
				while (cp > b) {
					usart0_putchar('\b', stream);
					usart0_putchar(' ', stream);
					usart0_putchar('\b', stream);
					cp--;
				}
				break;
				
			  case 'w' & 0x1f:
				while (cp > b && cp[-1] != ' ') {
					usart0_putchar('\b', stream);
					usart0_putchar(' ', stream);
					usart0_putchar('\b', stream);
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
*/

void usart0_init(void) {
	power_usart0_enable();

	q_init(&tx_q);

	/* Set baud rate (12bit) */
	#define BAUD 19200
	#include <util/setbaud.h>
	UBRR0   = UBRR_VALUE;
	UCSR0A &= (uint8_t)~(1<<UDRE0);
	#if USE_2X
	UCSR0A |=  (1 << U2X0);
	#else
	UCSR0A &= (uint8_t) ~(1 << U2X0);
	#endif
	// Set frame format: 8data, 1stop bit
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
	
	// Enable receiver and transmitter
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);
	// Enable rx inter
	UCSR0B |= (1<<RXCIE0);

	stdout=&usart0_stdout;
	stderr=&usart0_stderr;
}

void usarts_init(void) {
	usart0_init(); //std[in/out/err]
	//usart1_init(); // Not implimented
}

ISR(USART0_UDRE_vect) {	
	if (tx_q.ct>0) // !q_empty(&tx_q)
		UDR0 = q_pop(&tx_q);
	if (tx_q.ct==0)// q_empty(&tx_q)
		disable_usart0_tx_inter();			
}

ISR(USART0_RX_vect) {
	recieved = UDR0;
	if (recieved == '*')
		q_init(&tx_q);
}

