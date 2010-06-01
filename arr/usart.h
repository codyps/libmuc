/*
USART
*/

#ifndef _USART_H_
#define _USART_H_

#include <stdint.h>

#define BAUD 38400
#include <util/setbaud.h>

void usart_init(void);

void usart_flush_rx( void );
void usart_flush_tx( void );
volatile uint8_t usart_msg;
volatile bool usart_echo;

#endif
