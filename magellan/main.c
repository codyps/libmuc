/*
  For the ATMEGA164PV i have on the breadboard
 */

#include "defines.h"

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"
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
	power_all_disable();
	clock_init();
	MCUCR|=(1<<JTD); // Disable JTAG
	usarts_init();
	timers_init();
	twi_init();
	sei();	
	printf_P(PSTR("\nmain: init:\t[done]\n\n"));	
}

void  print_bin(uint8_t inp) {
	for(int8_t j=7; j>=0; --j) {
	   	printf("%c",((inp&(1<<j))>>j)+'0');
	}
}

ISR(BADISR_vect){
	fprintf_P(stderr,PSTR("\n\tError: Invalid Interupt\n"));
}

int main(void){ 	
	init();
		
	for(;;) {

	}
	return 0;
}
