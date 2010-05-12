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


/*
ISR(BADISR_vect){
}
*/

void init(void) {
	power_all_disable();

	sei();
  
	printf_P(PSTR("\n\n[main init done]\n\n"));
}

int main(void) {
	init();
	for(;;) {

	}
   	return 0;
}

