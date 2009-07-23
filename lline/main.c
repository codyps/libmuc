/*
 * ATtiny861
 	general initialization and main loop
 */

#include "defines.h"
#include "common.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
}

int main(void) {
	init();

	uint16_t adc_val[ADC_CHANNEL_CT];
	
     
    	for(;;) {
		if (adc_new_data) {
			adc_new_data = false;
			memcpy(adc_val, (uint16_t *) adc_values,sizeof(adc_val));
			for (uint8_t i = 0; i < ADC_CHANNEL_CT; i++) {			
				spi_putchar(i+'0');
				spi_putchar(' ');
				spi_putchar(':');
				spi_puth2(adc_val[i]);
				spi_putchar('\t');
			}
			spi_putchar('\n');
		}
		_delay_ms(200);
	}
	return 0;
}

