/*
USART
*/

#ifndef _USART_H_
#define _USART_H_

#define BAUD 38400
#include <util/setbaud.h>

//#include <stdio.h>
//#include "defines.h"
#include "queue.h"

QUEUE_BASE_T _tx_buffer[QUEUE_SZ];
queue_t tx_q;

void usart1_init(void);

#endif
