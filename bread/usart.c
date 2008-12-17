#include <stdio.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "queue.h"
#include "defines.h"
#include "usart.h"



static int usart0_putchar(char c, FILE *stream);
static FILE usart0_stdout = FDEV_SETUP_STREAM(usart0_putchar, NULL,_FDEV_SETUP_WRITE);


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

static int usart0_putchar(char c, FILE *stream) {		if (c == '\n')
		usart0_putchar('\r', stream);
  
	#ifndef	USART_QUEUE
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	#else 	
	while (q_full(&tx_q));
	disable_usart0_tx_inter();
	q_push(&tx_q,c);	
	enable_usart0_tx_inter();
	#endif

	return 0;
}

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
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
	
	/* Enable receiver and transmitter */
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);
	/* Enable r/t interupts, hangles input when used with some buffering functions */
	#ifdef USART_QUEUE	
	//enable_usart0_tx_inter();	
	#endif
	UCSR0B |= (1<<RXCIE0);//(1<<UDRE0)
		
	stdout=&usart0_stdout;
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

