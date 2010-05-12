/*
 * Arduino
 */

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <util/parity.h> 
#include <util/delay.h>

#include "usart.h"

/*
ISR(BADISR_vect){
}
*/

void init(void) {
	power_all_disable();

	usart_init();

	sei();
  
	printf_P(PSTR("\n\n[main init done]\n\n"));
}

int main(void) {
	init();
	for(;;) {
		printf("Hello\n");
		_delay_ms(100);
	}
   	return 0;
}

