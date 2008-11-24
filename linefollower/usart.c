#include "usart.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/power.h>

static int usart_putchar(char c, FILE *stream);
static FILE usart_stdout = FDEV_SETUP_STREAM(usart_putchar, NULL,_FDEV_SETUP_WRITE);


/*
static int usart_getchar(char c, FILE *stream) {
	return 1;
}
*/

static int usart_putchar(char c, FILE *stream) {

  if (c == '\n')
	usart_putchar('\r', stream);
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

ISR(USART0_RX_vect) {
}

ISR(USART0_UDRE_vect) {
}

ISR(USART0_TX_vect) {
}


void usart_init(void) {
	power_usart0_enable();

	/* Set baud rate (12bit) */
	UBRR0 = UBRR_VALUE;
	#if USE_2X
	UCSR0A |= (1 << U2X0);
	#else
	UCSR0A &= ~(1 << U2X0);
	#endif
	/* Double the uart clock */
	//UCSR0A |=(1<<U2X0);
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)	;
	/* Enable r/t interupts, hangles input when used with some buffering functions */
	UCSR0B =|(1<<RXCIE0)|(1<<TXCIE0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
	
	stdout=&usart_stdout;
}

