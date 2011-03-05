#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <stdio.h>

#include "clock.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#define unused		__attribute__((unused))

#define DDR(x)	_DDR(x)
#define _DDR(x) DDR##x
#define PORT(x) _PORT(x)
#define _PORT(x) PORT##x
#define PIN(x)	_PIN(x)
#define _PIN(x) PIN##x

#define MAX(x, y) ((x) > (y)?(x):(y))
#define MAX4(a, b, c, d) MAX(MAX(a,b),MAX(c,d))
#define MAX6(a,b,c,d,e,f) MAX(MAX4(a,b,c,d),MAX(e,f))
#define MAX8(a,b,c,d,e,f,g,h) MAX(MAX4(a,b,c,d),MAX4(e,f,g,h))

#define MIN(x,y) ((x) < (y)?(x):(y))
#define MIN4(a, b, c, d) MIN(MIN(a,b),MIN(c,d))
#define MIN6(a,b,c,d,e,f) MIN(MIN4(a,b,c,d),MIN(e,f))
#define MIN8(a,b,c,d,e,f,g,h) MIN(MIN4(a,b,c,d),MIN4(e,f,g,h))

#define CT(x) ARRAY_SIZE(x)
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(*(x)))

#define ntohs bswap16
#define htons bswap16
static inline uint16_t bswap16(uint16_t s)
{
	union u {
		struct s {
			uint8_t a, b;
		} s;
		uint16_t x;
	} u;

	u.x = s;
	uint8_t tmp = u.s.a;
	u.s.a = u.s.b;
	u.s.b = tmp;
	return u.x;
}

#define htonl bswap32
#define ntohl bswap32
//#define bswap32(x) ((bswap16(x & 0xffff)) << 16 | (bswap16(x >> 16)))
static inline uint32_t bswap32(uint32_t s)
{
	union u {
		struct s {
			uint8_t a, b, c, d;
		} s;
		uint32_t x;
	} u;

	u.x = s;
	uint8_t tmp = u.s.a;
	u.s.a = u.s.d;
	u.s.d = tmp;

	tmp = u.s.b;
	u.s.b = u.s.c;
	u.s.c = tmp;
	return u.x;
}

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
