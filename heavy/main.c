/*
 * Axon (atmega640)
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

#include "util.h"

#include "../common/spi_io.h"
#include "queue.h"

void init(void) {
	power_all_disable();

  clock_prescale_set(clock_div_1);

  spi_io_init();
  sei();  
  stdout = spi_io;

	printf_P(PSTR("\n\n[main init done]\n\n"));
}

/*
ISR(BADISR_vect){
}
*/

int main(void) {
	init();
	for(;;) {
        
    printf_P(PSTR("helloo...\n"));
    printf_P(PSTR("junk     \n"));
		_delay_ms(400);
	}
	return 0;
}

