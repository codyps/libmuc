
#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "queue.h"

/* 
ATtinyX61:
DI -> 0
DO -> 1
CLK-> 2
*/

#define I_DI  0
#define I_DO  1
#define I_CLK 2

#define NEGATIVE 1
#define POSITIVE 0
#define SPI_EDGE POSITIVE

#define USI_PORT B

#define DDR(x)	_DDR(x)
#define _DDR(x) DDR##x
#define PORT(x) _PORT(x)
#define _PORT(x) PORT##x

uint8_t tx_b[24],	rx_b[8];
queue_t tx,		rx;

#define spi_isr_on()	(USICR |= (1<<USIOIE))
#define spi_isr_off()	(USICR &= (uint8_t)~(1<<USIOIE))

void spi_io_init(void) {
     power_usi_enable();

     //USISR = 0b11100000; // Clear all flags and counter (not needed if only called once)

	#if (USI_PORT==A)
     USIPP |= (1<<USIPOS); // Use port A instead of B
	#else
     //USIPP &= (uint8_t)~(1<<USIPOS); // Use port A instead of B     	
     #endif

	DDR(USI_PORT) |= (1<<I_DO);
	DDR(USI_PORT) &= (uint8_t)~((1<<I_DI)|(1<<I_CLK));
	PORT(USI_PORT) &= (uint8_t)~((1<<I_DI)|(1<<I_CLK));


     USICR = (0<<USISIE) |			// Start interupt (2w only)
          (1<<USIOIE) |				// Ovf interupt (to refil register)
          (1<<USIWM1) | (0<<USIWM0) |	// 10 = 3w, 00 = disable, 01&11 = 2w
          (1<<USICS1) | (SPI_EDGE<<USICS0) | // 10 = positive edge, 11 = neg.
          (1<<USICLK) |				// 4bit timer : 1 = external, 0 = sw
          (0<<USITC );				// Clock toggle

     
     q_init(&rx, rx_b, sizeof(rx_b));
     q_init(&tx, tx_b, sizeof(tx_b));
}

ISR( SIG_USI_OVERFLOW ) {
     USISR = (1<<USIOIF); // Clear interupt flag and counter.

     //rx
     if (USIBR != 0 && !q_full(&rx)) {
          q_push(&rx,USIBR);
     }

	//tx
     if (!q_empty(&tx)) {
		USIDR = q_pop(&tx);
	}
	else USIDR = 0;
}

int spi_putc(int c, FILE * stream) {
	//while(q_full(&tx));	// delay if queue full
	spi_isr_off();
	uint8_t r = q_push(&tx,(uint8_t)c);
	spi_isr_on();
	if (r==0)	
		return c;
	else
		return EOF;
}

int spi_getc(FILE * stream) {
	//while(q_empty(&rx));
	int r;
	spi_isr_off();
	if (q_empty(&rx))
		r = EOF;
	else
		r = q_pop(&rx);
	spi_isr_on();
	return r; 

}

void spi_puts(char * string) {
	spi_isr_off();
	while(*string) {
		while(q_full(&tx)) {
			spi_isr_on();
			asm("nop");	
			asm("nop");
			spi_isr_on();
		}
		q_push(&tx,*string);
		string++;
	}
	spi_isr_on();
}


