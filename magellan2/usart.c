#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "queue.h"
#include "usart.h"

static int usart1_putchar(char c, FILE *stream);
static FILE usart1_stdout = FDEV_SETUP_STREAM(usart1_putchar, NULL ,_FDEV_SETUP_WRITE);
static int usart1_putchar_direct(char c, FILE *stream);
static FILE usart1_stderr = FDEV_SETUP_STREAM(usart1_putchar_direct, NULL,_FDEV_SETUP_WRITE);


static void enable_usart1_tx_inter(void)  { UCSR0B|=(1<<UDRIE0); }
static void disable_usart1_tx_inter(void) { UCSR0B&=(uint8_t)~(1<<UDRIE0); }


static int usart1_putchar_direct(char c, FILE *stream) {
        if (c == '\n')
                usart1_putchar_direct('\r', stream);
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
        return 0;
}

static int usart1_putchar(char c, FILE *stream) {		if (c == '\n')
		usart1_putchar('\r', stream);

	while (q_full(&tx_q)) {
		enable_usart1_tx_inter();
		sei();
	}
	//disable_usart1_tx_inter();
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
	q_push(&tx_q,c);	
	}
	enable_usart1_tx_inter();

	return 0;
}

static void usart1_init(void) {
	power_usart1_enable();

	q_init(&tx_q, _tx_buffer, QUEUE_SZ);
	/* Set baud rate (12bit) */
	#define BAUD 38400
	#include <util/setbaud.h>
	UBRR1   = UBRR_VALUE;
	UCSR1A &= (uint8_t)~(1<<UDRE1);
	#if USE_2X
	UCSR1A |=  (1 << U2X1);
	#else
	UCSR1A &= (uint8_t) ~(1 << U2X1);
	#endif
	// Set frame format: 8data, 1stop bit
	UCSR1C = (1<<UCSZ10)|(1<<UCSZ11);
	
	// Enable receiver and transmitter
	UCSR1B = 0;	
	UCSR1B|= (1<<TXEN1); // Recieve??
	UCSR1B|= (1<<RXEN1); // Transmit??

	stderr=stdout=&usart1_stdout;
	stddirect = &usart1_stderr;
}

void usarts_init(void) {
	usart1_init(); //std[in/out/err]
}

ISR(USART1_UDRE_vect) {	
	if (tx_q.ct>0) // !q_empty(&tx_q)
		UDR0 = q_pop(&tx_q);
	if (tx_q.ct==0)// q_empty(&tx_q)
		disable_usart1_tx_inter();
}

