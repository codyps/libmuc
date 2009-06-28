#ifndef _COMMON_H_
#define _COMMON_H_

#include "clock.h"

inline void memset_16(uint16_t * dest, uint16_t val, uint8_t len) {
        do { 
		len--;
		dest[len] = val;
	} while (len!=0);
}

inline void print_bin(uint8_t inp, FILE * stream) {
	for(int8_t j=7; j>=0; --j) {
	   	fputc(((inp&(1<<j))>>j)+'0',stream);
	}
}

#endif
