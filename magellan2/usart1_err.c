#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "queue.h"
#include "usart1.h"
/*
const char C_ESC = '\x1b';
const char C_STA = '[';

const char C_FG  = '3';
const char C_BG  = '4';

const char C_RED = '1';
const char C_RED = '2';
const char C_RED = '3';
const char C_RED = '4';
const char C_RED = '5';
const char C_RED = '6';
const char C_RED = '7';

const char C_STO = 'm';
*/

QUEUE_BASE_T _tx_buffer[QUEUE_SZ];
queue_t tx_q;

const char C_RESET[] = "\x1b[0m";
const char C_RED[] = "\x1b[31m";
	

//static int usart1_putchar_queue_ts3a(char c, FILE *stream);
static int usart1_putchar_queue_ts3b(char c, FILE *stream);

static FILE usart1_io_queue_out = FDEV_SETUP_STREAM(usart1_putchar_queue_ts3b, NULL ,_FDEV_SETUP_WRITE);
static FILE usart1_io_queue_err = FDEV_SETUP_STREAM(usart1_putchar_queue_ts3b, NULL ,_FDEV_SETUP_WRITE);

static int usart1_putchar_direct(char c, FILE *stream);
static FILE usart1_io_direct = FDEV_SETUP_STREAM(usart1_putchar_direct, NULL,_FDEV_SETUP_WRITE);

static void usart1_udre_inter_on(void)  { UCSR1B|=(1<<UDRIE1); }
static void usart1_udre_inter_off(void) { UCSR1B&=(uint8_t)~(1<<UDRIE1); }


static int usart1_putchar_direct(char c, FILE *stream) {
	if (c == '\n')
		putc('\r', stream);
	loop_until_bit_is_set(UCSR1A, UDRE1);
	UDR1 = c;
	return 0;
}

/*
static int usart1_putchar_queue_ts3a(char c, FILE *stream) {	
	if (c == '\n')
		putc('\r', stream);

	queue_t * q;
	if (stream == stderr)
		q = &tx_err_q;
	else
		q = &tx_q;

	uint8_t sreg = SREG;
	cli();
	if (q_full(q)) {
		PORTB&=(uint8_t)~(1<<6);
		usart1_udre_inter_on();
		while ( q_full(q) ) { 
			sei();
			asm(
				"nop" "\n\t"
				"nop" "\n\t"
			);
			cli();
		}
		PORTB|=(1<<6);
		q_push(q,c);
		SREG=sreg;
		return 0;
	}	
	usart1_udre_inter_off();
	q_push(q,c);
	usart1_udre_inter_on();
	SREG=sreg;
	return 0;
}
*/

static int usart1_putchar_queue_ts3b(char c, FILE *stream) {	
	static bool stderr_started = false;
	if (c == '\n') {
		putc('\r', stream);
		if ( (stream != stderr) && stderr_started) {
			fputs(C_RESET,stream);
			stderr_started = false;
		}
	}

	queue_t * q =&tx_q;
	if ( (stream == stderr) && !stderr_started) {
		stderr_started = true;
		fputs(C_RED, stream);
	}
		

	uint8_t sreg = SREG;
	cli();
	if (q_full(q)) {
		fputs("\nQueue Full",io_direct);
		usart1_udre_inter_on();
		while ( q_full(q) ) { 
			sei();
			asm(
				"nop" "\n\t"
				"nop" "\n\t"
			);
			cli();
		}
		q_push(q,c);
		SREG=sreg;
		return 0;
	}	
	usart1_udre_inter_off();
	q_push(q,c);
	usart1_udre_inter_on();
	SREG=sreg;
	return 0;
}

void usart1_init(void) {
	power_usart1_enable();

	q_init(&tx_q, _tx_buffer, sizeof(_tx_buffer));
	//q_init(&tx_err_q, _tx_err_buffer, sizeof(_tx_err_buffer));
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

	fputs(C_RESET,&usart1_io_queue_out);
	
	io_queue = &usart1_io_queue_out;
	io_direct = &usart1_io_direct;
	stdout = io_queue;
	stderr = &usart1_io_queue_err;
	io_isr = io_direct;
	io_init = io_direct;
}
/*
ISR(USART1_UDRE_vect) {	
	static queue_t * q1 = &tx_q;
	static queue_t * q2 = &tx_err_q;
	
	if (!q_empty(q1)) {
		char c = q_pop(q1);
		UDR1 = c;
		if (c == '\n' && !q_empty(q2)) {
			queue_t * t = q1;
			q1 = q2;
			q2 = t;
		}
	}
	else if (!q_empty(q2)) {
		// Wait for '\n' or swap imediatly??
		UDR1 = '\n';
		queue_t * t = q1;
		q1 = q2;
		q2 = t;
	}
	if (q_empty(q1) && q_empty(q2))// q_empty(&tx_q)
		usart1_udre_inter_off();
}
*/
ISR(USART1_UDRE_vect) {	
	if (tx_q.ct>0) // !q_empty(&tx_q)
		UDR1 = q_pop(&tx_q);
	if (tx_q.ct<=0)// q_empty(&tx_q)
		usart1_udre_inter_off();
}
