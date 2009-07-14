
#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "common.h"
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

#define USI_PORT B

#define NEGATIVE 1
#define POSITIVE 0
#define SPI_EDGE POSITIVE

uint8_t tx_b[32],	rx_b[32];
queue_t tx,		rx;

#define spi_isr_on()	(USICR |= (1<<USIOIE))
#define spi_isr_off()	(USICR &= (uint8_t)~(1<<USIOIE))

void spi_io_init(void) {
     power_usi_enable();

	q_init(&rx, rx_b, sizeof(rx_b));
     q_init(&tx, tx_b, sizeof(tx_b));

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

}

ISR( SIG_USI_OVERFLOW ) {
     USISR = (1<<USIOIF); // Clear interupt flag and counter.
	
	//transmit (tx)
     if (!q_empty(&tx)) {
		USIDR = q_pop(&tx);
	}
	else USIDR = 0;

     //recieve (rx)
     if (USIBR != 0 && !q_full(&rx)) {
          q_push(&rx,USIBR);
     }

	/*// Alternate if we don't care about losing old data.
	if (USIBR != 0) {
          q_push_o(&rx,USIBR);
     }
	*/

}

int spi_putc(int c, FILE * stream) {

	int r;	
	spi_isr_off();
	if (q_full(&tx))
		r = EOF;
	else 
		r = q_push(&tx,(uint8_t)c);
	spi_isr_on();
	return r;

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

