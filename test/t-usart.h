#ifndef T_USART_H_
#define T_USART_H_

#include <stdint.h>

#define UDRIE0 0
#define RXCIE0 1
#define RECIE0 2
uint8_t UCSR0B;


uint8_t UCSR0A;
uint8_t UBRR0;
uint8_t UCSR0C;
uint8_t UCSZ01;

uint8_t UCSZ00;
uint8_t TXEN0;
uint8_t UDR0;
uint8_t RXEN0;

#define power_usart0_enable()


#define ISR(X) void X(void)

#endif
