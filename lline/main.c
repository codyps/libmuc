/*
 * ATtiny861
 	general initialization and main loop
 */

#include "defines.h"
#include "common.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#include <avr/pgmspace.h>

#include "spi_io.h"
#include "adc.h"

void init(void) {
	power_all_disable();

	debug_led_init();

     clock_prescale_set(clock_div_1);

	spi_io_init();

	adc_init();

	sei();
}

ISR(BADISR_vect){
     for(;;) {
		debug_led_flash(2000,300);
	}
}

int main(void) {
	init();
     
    	for(;;) {
          spi_puts("HELLO\n");
		_delay_ms(1000);
		debug_led_flash(1000,500);
	}
	return 0;
}

