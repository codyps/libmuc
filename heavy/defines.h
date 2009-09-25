#ifndef _DEFINES_H_
#define _DEFINES_H_ 

#include "clock.h"
#include "debug.h"
#include <stdio.h>

//#define USE_I2C

#define F_CPU 8000000

FILE * io_queue;
FILE * io_direct;

FILE * io_isr;
FILE * io_init;

#endif
