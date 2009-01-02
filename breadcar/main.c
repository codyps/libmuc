/*
  attiny 861
 */

#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "timer.h"

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
	
}

void init(void) {
	//power_all_disable();
	clock_init();
	timers_init();
	sei();
	DEBUG_LED_DDR|=(1<<DEBUG_LED_POS)|(1<<ERROR_LED_POS);
	DEBUG_LED_FLIP;
	fprintf_P(stderr,PSTR("\nmain: init:\t[done]\n\n"));	
}

void  print_bin(uint8_t inp) {
	for(int8_t j=7; j>=0; --j) {
	   	printf("%c",((inp&(1<<j))>>j)+'0');
	}
}

ISR(BADISR_vect){
	//fprintf_P(stderr,PSTR("\n\tError: Invalid Interupt\n"));
	ERROR_LED_FLIP;
}

int main(void){ 	
	init();
	
	for(;;) {
		//DEBUG_LED_FLIP;
		_delay_ms(100);

	}
	return 0;
}
