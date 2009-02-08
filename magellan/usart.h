/*
USART
*/

#ifndef _USART_H_
#define _USART_H_

//#include <stdio.h>
//#include "defines.h"
#include "queue.h"

QUEUE_BASE_T _rx_buffer[QUEUE_SZ];
QUEUE_BASE_T _tx_buffer[QUEUE_SZ];
queue_t tx_q;
queue_t rx_q;

void usarts_init(void);
static void usart0_init(void);

#endif
