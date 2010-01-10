/* 
ATtinyX61:
DI -> 0
DO -> 1
CLK-> 2
*/

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

static inline void spi_isr_on (void)  {USICR |= (1<<USIOIE);}
static inline void spi_isr_off(void)  {USICR &= (uint8_t)~(1<<USIOIE);}

static void spi_io_init_hw(void) {
	power_usi_enable();

	// Clr the flags we interupt on + counter bits in same reg. 
	//  (default is not raised)
	//USISR = (1<<USIOIF);
 
	// Clr datareg to avoid junk data being sent out. (defaults to 0)
	//USIDR = 0;
	
	#define B 0
	#define A 1

	#if ( SPI_PORT == B )
  	//#warning Using Port  B (default)
	//USIPP &= (uint8_t)~(1<<USIPOS);
	#else
  	#error Using Port  A   
  	USIPP |= (1<<USIPOS);
	#endif
	
	#undef A
	#undef B

	DDR(SPI_PORT) |= (1<<SPI_MISO);
	DDR(SPI_PORT) &= (uint8_t)~((1<<SPI_MOSI)|(1<<SPI_CLK));
	PORT(SPI_PORT)&= (uint8_t)~( (1<<SPI_MOSI) | (1<<SPI_CLK) | (1<<SPI_MISO) );

	// To resolve SPI_EDGE
	#define NEGATIVE 1
	#define POSITIVE 0
  
	USICR = (0<<USISIE) | // Start IE(2w), CLK edge IE(3w)
		(1<<USIOIE) | // Ovf interupt (to refil register)
		(0<<USIWM1) | (1<<USIWM0) | // 01 = 3w, 00 = disable, 10&11 = 2w
		(1<<USICS1) | (SPI_EDGE<<USICS0) | // 10 = positive edge, 11 = neg.
		(0<<USICLK) | // 4bit timer : 0 = external, 1 = sw
		(0<<USITC );  // Clock toggle

	#undef NEGATIVE
	#undef POSITIVE
}

ISR( SIG_USI_OVERFLOW ) {
	USISR = (1<<USIOIF); // Clear interupt flag and counter.
	
	//transmit (tx)
	if (!list_empty(&tx)) {
		USIDR = list_pop_back(&tx);
	}
	else USIDR = 0;

	//recieve (rx)
	uint8_t in = USIBR;
	if (in != 0 && !list_full(&rx)) {
	        list_push_front(&rx,in);
		if (in == '\n')
			spi_io_rx_nl++;
	}

	/*// Alternate if we don't care about losing old data.
	if (USIBR != 0) {
		list_push_front_o(&rx,USIBR);
	}
	*/
}
