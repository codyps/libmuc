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

uint8_t sspi_xfer_byte(uint8_t data)
{
	uint8_t ret;
	SPDR = data;
	while(!(SPSR & (1 << SPIF)))
		;
	ret = SPDR;
	return ret;
}

/* @max spi clock we have 16 ticks per byte transfered
 * (F_SPI <= F_CLK / 2) */
void sspi_xfer(uint8_t *dst, uint8_t *src, uint16_t len)
{
	uint8_t *end = src + len;
	while (src != end) {
		SPDR = *src;
		src ++;
		while(!(SPSR & (1 << SPIF)))
			;
		*dst = SPDR;
		dst ++;
	}
}

#if 0
void sspi_xfer_asm(uint8_t *dst, uint8_t *src, uint16_t len)
{
	/* AVR specific constraints:
	 * 'I' : 6 bit const (io address)
	 * '
	 */
	uint8_t *end = src + len;
	asm(
	    /* while (src != end) */
	    "CP %A[src], %A[end]"        "\n\t"
	    "BRNE start_%="              "\n\t"
	    "CP %B[src], %B[end]"        "\n\t"
	    "BREQ end_%="                "\n\t"
	    "start_%=:"                  "\n\t"
	    /* SPDR = *src; src++; */
	    "LD %__tmp_reg__, %[src]+"   "\n\t"
	    "OUT %[spdr], %__tmp_reg__"  "\n\t"
	    /* while(!(SPSR & (1 << SPIF))); */
	    "SBIS %[spsr], %[spif]"      "\n\t"
	    "RJMP -2"                    "\n\t"
	    /* *dst = SPDR; dst++; */
	    "IN %__tmp_reg__, %[spdr]"   "\n\t"
	    "ST %[dst]+, %__tmp_reg"     "\n\t"
	    /* while(src != end) */
	    "CP %[src], %[end]"          "\n\t"
	    "BRNE start_%="              "\n\t"
	    "end_%=:"                    "\n\t"
	    : /* results */
	    : /* inputs  */ [dst]  "w" (dst),
	                    [src]  "w" (src),
			    [end]  "w"  (end),
			    [spdr] "I" (_SFR_IO_ADDR(SPDR)),
			    [spsr] "I" (_SFR_IO_ADDR(SPSR)),
			    [spif] "I" (SPIF)
	    : /* clobber */ "r0"
	   );
}
#endif
