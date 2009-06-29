/*
 * ATtiny861
 	general initialization and main loop
 */

#include "defines.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void init(void) {
	power_all_disable();

	sei();

	printf_P(PSTR("\n\n[main init done]\n\n"));
}

ISR(BADISR_vect){
	fprintf_P(io_isr,PSTR("\n[error] bad isr\n"));
}

int main(void) {
	init();

	for(;;) {
		_delay_ms(1000);
	}
	return 0;
}

