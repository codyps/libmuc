#ifndef _DEFINES_H_
#define _DEFINES_H_ 

#include "debug.h"
#include <stdio.h>

#define F_CPU 16000000

#define CLICKS_MS(_ms_) ( (uint16_t) ( F_CPU * (_ms_) / 1000 ) )
#define CLICKS_US(_us_) ( (uint16_t) ( F_CPU / 1000000 * (_us_) ) ) // 1000000 = 1e6

FILE * io_queue;
FILE * io_direct;

FILE * io_isr;
FILE * io_init;

#endif
