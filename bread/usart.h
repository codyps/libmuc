/*
USART
*/

#ifndef _USART_H_
#define _USART_H_

#include <stdio.h>
#include "defines.h"

#define BAUD 9800
#include <util/setbaud.h>


void usart_init(void);


#endif