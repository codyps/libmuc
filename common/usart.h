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

/* non-blocking input mechanizms */
bool usart_hasc(void);
char usart_getc(void);

void usart_flush_rx_to(char end_delim);
void usart_flush_rx(void);
void usart_flush_tx(void);

/* returns true if a new line has been recieved
 * expects the user to read up to that newline
 */
bool usart_new_msg(void);

#endif
