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

#include <util/parity.h>
#include <util/delay.h>

#include "util.h"

#include "bus/spi_io.h"

#include "card.h"

/*
ISR(BADISR_vect){
}
*/

void init(void)
{
	power_all_disable();

	clock_prescale_set(clock_div_1);
	card_init();
	spi_io_init();
	sei();

	stdout = spi_io;

	printf_P(PSTR("\n\n[main init done]\n\n"));
}

int main(void)
{
	init();
	for (;;) {
		card_spin();
	}
	return 0;
}
