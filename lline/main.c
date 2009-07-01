/*
 * ATtiny861
 	general initialization and main loop
 */

#include "defines.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "spi_io.h"

void init(void) {
	power_all_disable();

     clock_prescale_set(clock_div_1);

	spi_io_init();

	sei();

}

ISR(BADISR_vect){
     
}

int main(void) {
	init();
     
    	for(;;) {
          spi_puts("HELLO WORLD\n");
		_delay_ms(1000);
	}
	return 0;
}

