#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <stdio.h>

#include "clock.h"


#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define DDR(x)	_DDR(x)
#define _DDR(x) DDR##x
#define PORT(x) _PORT(x)
#define _PORT(x) PORT##x
#define PIN(x)	_PIN(x)
#define _PIN(x) PIN##x

#define MIN(x,y) (((x)<(y))?(x):(y))

#define CT(x) ARRAY_SIZE(x)
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(*(x)))

static inline void barrier(void)
{
	asm volatile ("":::"memory");
}

static inline void print_bin(uint8_t inp, FILE * stream) {
	for(int8_t j=7; j>=0; --j) {
		fputc(((inp&(1<<j))>>j)+'0',stream);
	}
}

#endif
