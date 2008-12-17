/*
  For the ATMEGA164PV i have on the breadboard
 */

#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>


#include "usart.h"
#include "timer.h"
#include "card_reader.h"

void clock_init(void) {
	
	#if   (F_CPU == 1000000)
		clock_prescale_set(clock_div_8);	
	#elif (F_CPU == 2000000)
		clock_prescale_set(clock_div_4);	
	#elif (F_CPU == 4000000)
		clock_prescale_set(clock_div_2);	
	#elif (F_CPU == 8000000)
		clock_prescale_set(clock_div_1);
	#else
		#error "F_CPU Unrecognized"
	#endif
	
	// Default
	//OSCCAL=0x84;

	// Posible
	//OSCCAL=0x8C;
}

void init(void) {
	DDRB |=(1<<3)|(1<<4);	
	power_all_disable();
	clock_init();
	MCUCR|=(1<<JTD); // Disable JTAG
	usart0_init();
	timers_init();
	card_reader_init();
	printf_P(PSTR("\nmain: init:\t[done]\n\n"));
	sei();
}

void  print_bin(uint8_t inp) {
	for(int8_t j=7; j>=0; --j) {
	   	printf("%c",((inp&(1<<j))>>j)+'0');
	}
}

static char dirtoc(dir_t d) {
	if	(d==UP)
		return 'U';
	else if (d==DN)
		return 'D';
	else
		return '?';
}

ISR(BADISR_vect){
	fprintf_P(stderr,PSTR("\n\tError: Invalid Interupt\n"));
}

int main(void){ 	
	init();
	
	for(;;) {
		/*
		uint8_t data, clk, motion, fdet, edet;
		data	= (PINB&(1<<1))>>1; //always high ?
		clk	= (PINB&(1<<2))>>2; //always high ?
		motion	= (PINC&(1<<5))>>5; //always low  ?
		fdet	= (PINC&(1<<7))>>7; //changes
		edet	= (PINC&(1<<6))>>6; //changes
		printf("\nDATA: %d; CLK: %d; MOTION: %d; FRONT DETECT: %d; END DETECT: %d",
			data, clk, motion, fdet, edet);
		_delay_ms(100);
		*/

	}
	return 0;
}
