#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "queue.h"
#include "usart.h"


static int usart0_putchar_direct(char c, FILE *stream);
static int usart0_putchar(char c, FILE *stream);
static FILE usart0_stdout = FDEV_SETUP_STREAM(usart0_putchar, NULL ,_FDEV_SETUP_WRITE);
static FILE usart0_stderr = FDEV_SETUP_STREAM(usart0_putchar_direct, NULL,_FDEV_SETUP_WRITE);

static void enable_usart0_tx_inter(void) {
	UCSR0B|=(1<<UDRIE0);
}
static void disable_usart0_tx_inter(void) {
	UCSR0B&=(uint8_t)~(1<<UDRIE0);
}

static int usart0_putchar(char c, FILE *stream) {		if (c == '\n')
		usart0_putchar('\r', stream);

	if (q_full(&tx_q)) {
		sei();
		enable_usart0_tx_inter();
	}
	while (q_full(&tx_q));
	disable_usart0_tx_inter();
	q_push(&tx_q,c);	
	enable_usart0_tx_inter();

	return 0;
}

/*
static char usart0_getchar(FILE *stream) {	
	while (q_empty(&rx_q));
	disable_usart0_rx_inter();
	char c = q_pop(&rx_q);	
	enable_usart0_rx_inter();

	return c;
}
*/

static int usart0_putchar_direct(char c, FILE *stream) {
	if (c == '\n')
		usart0_putchar_direct('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}


static void usart0_init(void) {
	power_usart0_enable();

	q_init(&tx_q, _tx_buffer, QUEUE_SZ);
	/* Set baud rate (12bit) */
	#define BAUD 38400
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
	UCSR0B = (1<<TXEN0);
	UCSR0B |=(1<<RXEN0);

	stderr=stdout=&usart0_stdout;
	//stderr=&usart0_stderr;
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

/*
ISR(USART0_RX_vect) {
	char c = UDR0;
	q_push(&tx_q,c);
}
*/
