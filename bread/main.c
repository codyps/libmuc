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
	clock_init();
	usart0_init();
	timers_init();
	printf_P(PSTR("\nmain: init:\t[done]\n\n"));
	sei();
}

static char dirtoc(dir_t d) {
	if		(d==UP)
		return 'U';
	else if (d==DN)
		return 'D';
	else
		return '?';
}

ISR(BADISR_vect){
	dpf_P(PSTR("\n\tError: Invalid Interupt"));
}

int main(void){ 
	
	init();
	DDRA |=0b11;
	PORTA|= (1<<0);
	PORTA&=~(1<<1);

	for(;;) {
		PINA|=(1<<0)|(1<<1);
		printf("\nA dir: %c\nB dir: %c",dirtoc(led_dir_A),dirtoc(led_dir_B));
		printf("\nLED_A: 0x%x; LED_B: 0x%x",LED_A,LED_B);
		_delay_ms(140);
		
	}
	return 0;
}
