#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

#include <stdint.h>
#include <avr/io.h>

static inline void spi_master_init(void)
{
	uint8_t ddr = DDR(SD_SPI_PORT);
	ddr &= ~(1 << SD_MISO_IX);
	ddr |=  (1 << SD_MOSI_IX) | (1 << SD_SCK_IX);
	DDR(SD_SPI_PORT) = ddr;

	/* Enable, Master, clk = fclk/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

static inline uint8_t spi_xfer(uint8_t b)
{
	SPDR = b;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

static inline spi_xfer_discard(uint8_t b)
{
	SPDR = b;
	loop_until_bit_is_set(SPSR, SPIF);
}


#endif
