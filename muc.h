#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <penny/penny.h>

#define DDR(x)	_DDR(x)
#define _DDR(x) DDR##x
#define PORT(x) _PORT(x)
#define _PORT(x) PORT##x
#define PIN(x)	_PIN(x)
#define _PIN(x) PIN##x

/* FIXME: relys on implimentation defined behvior: ordering of bits
 */
struct bits {
	uint8_t b0:1;
	uint8_t b1:1;
	uint8_t b2:1;
	uint8_t b3:1;
	uint8_t b4:1;
	uint8_t b5:1;
	uint8_t b6:1;
	uint8_t b7:1;
} __attribute__((__packed__));
#define SBIT(port,pin) ((*(volatile struct bits*)&PORT(port)).b##pin)
#define PIN_PIN(port,pin) ((*(volatile struct bits*)&PIN(port)).b##pin)
#define PIN_DDR(port,pin) ((*(volatile struct bits*)&DDR(port)).b##pin)

#define pin_init_output(port, bit) do {	\
	PIN_DDR(port, bit) = 1;		\
} while (0)

/* use SBIT() to pass data_pin and clk_pin
 * TODO:
 *  - allow specification of a maximum frequency.
 *  - use SPI/USI/USART hardware where possible. */
#define shift_out_msb_rising_edge(data_pin, clk_pin, byte) do {\
	uint8_t __som_i;				\
	for (__som_i = 0; __som_i < 8; __som_i++) {	\
		clk_pin = 0;				\
		data_pin = 0;				\
		if (byte & 0x80)			\
			data_pin = 1;			\
		byte <<= 1;				\
		clk_pin = 1;				\
	}						\
} while(0)

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

static inline void print_bin(uint8_t inp, FILE *stream) {
	int8_t j;
	for (j = 7; j >= 0; --j) {
		fputc(((inp&(1<<j))>>j)+'0',stream);
	}
}

#endif
