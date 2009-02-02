/*
	SPI
*/
#include "defines.h"
#include "spi.h"
#include <avr/io.h>

void init_spi() {
	//  MISO output, all others input.	
	DDR_SPI = (1<<DD_MISO);
	// Enable SPI, 
	SPCR|=(1<<SPE);
	// MSB first, SCK low when idle, Sample on leading edge,
	SPCR&=(uint8_t)~((1<<DORD)|(1<<CPOL)|(1<<CPHA);
}

static int spi_putchar(char c, FILE *stream) {
	loop_until_bit_is_set(SPSR, SPIF);
	SPIF = c;
	return 0;
}

/*
static char spi_getchar(FILE *stream){
	loop_until_bit_is_set(SPSR, SPIF);
	char c = SPIF;
	return c;
}
*/

/*
ISR(SIG_SPI){
	// Handle both sending and recieving of bytes.
	if (recieving)
		char c = SPIF;
		
	if (sending)
		SPIF=q_pop(&spi_tx_q);
}
*/
