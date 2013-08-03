#ifndef T_USART_H_
#define T_USART_H_

#include <stdint.h>
#include <stdio.h>

#define UCSR0A UCSR0A
uint8_t UCSR0A;
#define UCSR0B UCSR0B
uint8_t UCSR0B;
#define UCSR0C UCSR0C
uint8_t UCSR0C;
#define UBRR0 UBRR0
uint8_t UBRR0;
uint8_t UDR0;

#define power_usart0_enable()

#define ISR(X) void X(void)
#define FDEV_SETUP_STREAM(x, y, z) *stdin

/* UCSRA */
#define RXC0	7
#define TXC0	6
#define UDRE0	5
#define FE0	4
#define DOR0	3
#define PE0	2
#define U2X0	1
#define MPCM0	0

/* UCSRB */
#define RXCIE0	7
#define TXCIE0	6
#define UDRIE0	5
#define RXEN0	4
#define	TXEN0	3
#define UCSZ02	2
#define RXB80	1
#define TXB80	0

/* UCSRC */
#define URSEL0	7
#define UMSEL0	6
#define UPM01	5
#define UPM00	4
#define USBS0	3
#define UCSZ01	2
#define UCSZ00	1
#define UCPOL0	0

#endif
