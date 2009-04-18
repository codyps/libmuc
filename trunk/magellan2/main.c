/*
 * Axon (atmega640)
 	general initialization and main loop 
 */

#include "defines.h"

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#include "util.h"

#include "usart1.h"
#include "servo.h"
#include "i2c.h"
#include "i2c_HMC6343.h"

void init(void) {
	power_all_disable();
	
	usart1_init();
	twi_init();
	hmc6343_init_static();
	
	servo_init();

	DDRB |= (1<<6); // PB6 = status LED
//	debug_led_on;
//	debug_led_off;

	DDRG &= ~(uint8_t)(1<<5); //PG5 = button
	PORTG|= (1<<5);	// Needs pullups. Low when pressed.
	
	fprintf_P(io_init,PSTR("\n\n[main init done]\n\n"));	
	sei();
}

void print_bin(uint8_t inp, FILE * stream) {
	for(int8_t j=7; j>=0; --j) {
	   	fputc(((inp&(1<<j))>>j)+'0',stream);
	}
}

ISR(BADISR_vect){
	fprintf_P(io_isr,PSTR("\n[error] bad isr\n"));
}

int main(void) { 	
	init();
	
	i2c_start_xfer();
	
	uint16_t i = CLICKS_MS(1);
	uint8_t dir = 0;
	#define step (CLICKS_MS(1)/10)

	for(;;) {			
		if (head_data_updated == true) {
			head_data_updated = false;
			fprintf_P(stdout,PSTR("\nhead:%u pitch:%d roll:%d\n"),\
				head.head,head.pitch,head.roll);
			_delay_ms(50);
			i2c_start_xfer();
		}

		if (dir==0)
			i+=step;
		else
			i-=step;

		if (i> CLICKS_MS(2) || i< CLICKS_MS(1))
			dir=!dir;

		servo_set(i,0);	
		_delay_ms(1000);
	}
	return 0;
}

