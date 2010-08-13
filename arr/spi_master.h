#ifndef SPI_H_
#define SPI_H_
#include <stdint.h>

void spi_master_init(void);
uint8_t spi_xfer(uint8_t data);

enum spr_e {
	SPR_DIV4,
	SPR_DIV16,
	SPR_DIV64,
	SPR_DIV128
};

#define spi_set_clk(clk)                                               \
	do {                                                           \
		uint8_t spcr = SPCR;                                   \
		/* 0xfc = 0b11111100 */                                \
		SPCR = (spcr & ~((1 << SPR1) | (1 << SPR0))) | clk;    \
	} while(0)

#endif
