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

#define CT(x) (sizeof(x)/sizeof(*x))

inline uint8_t mod( uint8_t i1, uint8_t i2) {// i1%i2
     while( i1 >= i2 ) {
          i1 = (uint8_t) (i1 - i2);
     }
     return i1;
}

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


/* wip data structures

typedef union uint24 {
	struct {
		uint16_t msb16;
		uint8_t  lsb8;
	}
	struct {
		uint8_t msb8;
		uint16_t lsb16;
	}
	struct {
		uint8_t a,b,c;
	}
	uint8_t n[3];
} uint24_t;


typedef struct motor_st {
	volatile uint8_t * pwm_reg;
	volatile uint8_t * pwm_port;
	//volatile uint8_t * pwm_ddr; //pwm_port+1
	//volatile uint8_t * pwm_pin; //pwm_port+2
	uint8_t pwm_index;

	uint8_t in1_index;
	volatile uint8_t * in1_port;

	uint8_t in2_index;
	volatile uint8_t * in2_port;
} motor_t;
*/
#endif
