#include <stdint.h>
#include <avr/io.h>

#include "sspi_master_conf.h"
#include "sspi_master.h"

void sspi_master_init(void)
{
	/** IO Dir */

	/* MISO always input in master mode */
	DDR(SPI_PORT) |= (1 << SPI_MOSI_IX)|(1 << SPI_SCK_IX);

	/** SPI HW */
	SPSR = ((SPRx >> 2) << SPI2X);

	/* DORD 0 = MSB first.
	 * CPOL Leading Edge Trailing Edge
	 *    0 Rising       Falling
	 *    1 Falling      Rising
	 * CPHA Leading Edge Trailing Edge
	 *    0 Sample       Setup
	 *    1 Setup        Sample
	 */
	SPCR = (0 << SPIE) | (1 << SPE) | (0 << DORD) | (1 << MSTR)
		| (0 << CPOL) | (0 << CPHA) | (SPRx & 2);
}

uint8_t sspi_xfer(uint8_t data)
{
	uint8_t ret;
	SPDR = data;
	while(!(SPSR & (1 << SPIF)))
		;
	ret = SPDR;
	return ret;
}
