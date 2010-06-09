/*
USART
*/

#ifndef _USART_H_
#define _USART_H_

#include <stdbool.h>

#define BAUD 38400
#include <util/setbaud.h>

#define USART_RX_BUFF_SZ 64

void usart_init(void);

void usart_flush_msg(void);
void usart_flush_rx(void);
void usart_flush_tx(void);
bool usart_new_msg(void);
volatile bool usart_echo;

#endif
