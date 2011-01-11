/*
USART
*/

#ifndef _USART_H_
#define _USART_H_

#include "defines.h"
#include <stdint.h>

#define BAUD 38400
#include <util/setbaud.h>

void usart1_init(void);

void usart1_flush_rx( void );
void usart1_flush_tx( void );

#include "queue.h"
queue_t * _get_queue(uint8_t);

uint8_t usart_msg;

#endif
