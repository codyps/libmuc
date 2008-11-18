/*
  For the ATMEGA164PV i have on the breadboard
 */

#define F_CPU 1000000

#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include "usart.h"

void clock_init(void) {
	
	#if   F_CPU == 1000000
		clock_prescale_set(clock_div_8);	
	#elif F_CPU == 2000000
		clock_prescale_set(clock_div_4);	
	#elif F_CPU == 4000000
		clock_prescale_set(clock_div_2);	
	#elif F_CPU == 8000000
		clock_prescale_set(clock_div_1);	
	#else
		#error "F_CPU Unrecognized"
	#endif
	
	//Do something with OSCCAL ?
	
}

void init(void) {
	usart_init();
	OCR1A=OCR1B=0;
	timer1_init();
}

uint8_t led_dir = 0;

int main(void){ 
	init();
	DDRA|=0b11;
	for(;;) {
		/*_delay_ms(300);
		PORTA|=0b01;
		_delay_ms(300);
		PORTA|=0b11;
		_delay_ms(300);
		PORTA&=0b10;
		_delay_ms(300);
		PORTA&=0b01;*/
		if (OCR1A==1)		
		

		printf("You Bet\n");
	}
	return 0;
}
