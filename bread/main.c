/*
  For the ATMEGA164PV i have on the breadboard
 */

#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/power.h>
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
	//OCR1A=OCR1B=0;
	//timers_init();
}

int main(void){ 
	init();
	DDRA |=0b11;
	PORTA|= (1<<0);
	PORTA&=~(1<<1);

	for(;;) {
		PINA|=(1<<0)|(1<<1);
		/*
		printf("\n<<[[OSCCAL: 0x%x]]>>\n",OSCCAL);
		printf("\n<<[[UBRR0 : 0x%x]]>>\n",UBRR0 );
		printf("\n<<[[CLK PRESCAL: %d]]>>\n",clock_prescale_get() );	
		_delay_ms(70);
		OSCCAL++;
		_delay_ms(70);
		*/
		
	}
	return 0;
}
