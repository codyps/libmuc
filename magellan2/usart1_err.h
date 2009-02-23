/*
USART1 with out/err line buffering.
*/

#ifndef _USART_H_
#define _USART_H_

#include "defines.h"

#define BAUD 38400
#include <util/setbaud.h>

void usart1_init(void);

#endif
