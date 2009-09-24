
#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "common.h"
#include "queue.h"

#include "spi_io.h"

/* 
ATtinyX61:
DI -> 0
DO -> 1
CLK-> 2
*/

#define I_DI  0
#define I_DO  1
#define I_CLK 2

#define USI_PORT B

#define NEGATIVE 1
#define POSITIVE 0
#define SPI_EDGE POSITIVE

uint8_t tx_b[32],	rx_b[32];
queue_t tx = Q_INIT(tx_b);
queue_t rx = Q_INIT(rx_b);

#define spi_isr_on()	(USICR |= (1<<USIOIE))
#define spi_isr_off()	(USICR &= (uint8_t)~(1<<USIOIE))

#ifdef SPI_IO_STANDARD
int spi_putc(char c, FILE * stream);
int spi_getc(FILE * stream);
static FILE _spi_io = FDEV_SETUP_STREAM(spi_putc, spi_getc ,_FDEV_SETUP_RW);
#endif

void spi_io_init(void) {
     power_usi_enable();

	// Clr the flags we interupt on + counter bits in same reg. 
	//  (default is not raised)
     //USISR = (1<<USIOIF);
 
	// Clr datareg to avoid junk data being sent out. (defaults to 0)
	//USIDR = 0;

	
	#define B 0
	#define A 1
	#if ( USI_PORT == B )
	//#warning Using Port  B (default)
     //USIPP &= (uint8_t)~(1<<USIPOS);
	#else
	#error Using Port  A   
	USIPP |= (1<<USIPOS);
     #endif
	#undef A
	#undef B

	DDR(USI_PORT) |= (1<<I_DO);
	DDR(USI_PORT) &= (uint8_t)~((1<<I_DI)|(1<<I_CLK));
	PORT(USI_PORT)&= (uint8_t)~( (1<<I_DI) | (1<<I_CLK) | (1<<I_DO) );

     USICR = (0<<USISIE) |			// Start IE(2w), CLK edge IE(3w)
          (1<<USIOIE) |				// Ovf interupt (to refil register)
          (0<<USIWM1) | (1<<USIWM0) |	// 01 = 3w, 00 = disable, 10&11 = 2w
          (1<<USICS1) | (SPI_EDGE<<USICS0) | // 10 = positive edge, 11 = neg.
          (0<<USICLK) |				// 4bit timer : 0 = external, 1 = sw
          (0<<USITC );				// Clock toggle

	#ifdef SPI_IO_STANDARD
	spi_io = & _spi_io;
	#endif

}

ISR( SIG_USI_OVERFLOW ) {
     USISR = (1<<USIOIF); // Clear interupt flag and counter.
	
	//transmit (tx)
     if (!q_empty(&tx)) {
		USIDR = q_pop(&tx);
	}
	else USIDR = 0;

     //recieve (rx)
	uint8_t in = USIBR;
     if (in != 0 && !q_full(&rx)) {
          q_push(&rx,in);
		if (in == '\n')
			spi_io_rx_nl++;
     }

	/*// Alternate if we don't care about losing old data.
	if (USIBR != 0) {
          q_push_o(&rx,USIBR);
     }
	*/

}

#ifdef SPI_IO_STANDARD
int spi_putc(char c, FILE * stream) {
	int r;	
	spi_isr_off();
	//while(q_full(&tx));
	if (q_full(&tx))
		r = EOF;
	else 
		r = q_push(&tx,(uint8_t)c);
	spi_isr_on();
	return r;

}

int spi_getc(FILE * stream) {
	int r;
	spi_isr_off();
	//while(q_empty(&rx));
	if (q_empty(&rx))
		r = EOF;
	else
		r = q_pop(&rx);
	spi_isr_on();
	return r; 
}
#endif

int spi_getchar(void) {
	int r;
	spi_isr_off();
	//while(q_empty(&rx));
	if (q_empty(&rx))
		r = EOF;
	else
		r = q_pop(&rx);
	spi_isr_on();
	return r; 
}

void spi_puts(const char * string) {
	spi_isr_off();
	while(*string) {
		_spi_putchar(*string);
		string++;
	}
	spi_isr_on();
}

void spi_o_puts(const char * string) {
	spi_isr_off();
	while(*string) {
		if(q_full(&tx)) {
			spi_isr_on();
			asm(
				"nop\n\t"
				"nop\n\t"
				"nop"
			);			
			spi_isr_off();
		}
		q_push_o(&tx,*string);
		string++;
	}
	spi_isr_on();
}

static inline uint8_t hex2ascii(uint8_t hex) {
	hex = 0x0F & hex;
	hex = (uint8_t) (hex + '0');
	if (hex > '9')
		return (uint8_t) (hex + 7); // 7 characters between nums and caps.
	else
		return hex;
}

static inline void _spi_putchar(uint8_t ch) {
	// expects spi interupt disabled on entry, leaves it disabled on exit.
	while(q_full(&tx)) {
			spi_isr_on();
			asm(
				"nop\n\t"
				"nop"
			);			
			spi_isr_off();
		}
	q_push(&tx,ch);
}

void spi_putchar(uint8_t ch) {
	spi_isr_off();
	_spi_putchar(ch);
	spi_isr_on();
}

void spi_puth(uint8_t hex) {
	spi_isr_off();

	_spi_putchar( hex2ascii( (hex>>4 ) ) );
	_spi_putchar( hex2ascii( (hex>>0 ) ) );

	spi_isr_on();
}

void spi_puth2(uint16_t hex) {
	spi_isr_off();

	_spi_putchar( hex2ascii( (uint8_t)(hex>>12) ) );
	_spi_putchar( hex2ascii( (uint8_t)(hex>>8 ) ) );
	_spi_putchar( hex2ascii( (uint8_t)(hex>>4 ) ) );
	_spi_putchar( hex2ascii( (uint8_t)(hex>>0 ) ) );

	spi_isr_on();
}

