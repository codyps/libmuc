/*
  For the ATMEGA164PV i have on the breadboard
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

#include "usart1.h"
#include "i2c.h"
#include "i2c_HMC6343.h"

void init(void) {
	power_all_disable();
	usart1_init();
	twi_init();
	hmc6343_init_static();
	DDRB |= (1<<6); // PB6 = status LED
	PORTB |= (1<<6);
	DDRG &= ~(uint8_t)(1<<5); //PG5 = button
	PORTG|= (1<<5);	// Needs pullups. Low when pressed.
	fprintf_P(io_init,PSTR("\n\n[main init done]\n\n"));	
	sei();
}

void print_bin(uint8_t inp, FILE * stream) {
	for(int8_t j=7; j>=0; --j) {
	   	putc(((inp&(1<<j))>>j)+'0',stream);
	}
}


ISR(BADISR_vect){
	fprintf_P(io_isr,PSTR("\n[error] bad isr\n"));
}

int main(void) { 	
	init();

	i2c_start_xfer();
	for(;;) {			
		if (head_data_updated == true) {
			head_data_updated = false;
			fprintf_P(stdout,PSTR("\nhead:%u pitch:%d roll:%d\n"),\
				head.head,head.pitch,head.roll);
			_delay_ms(1000);		
			i2c_start_xfer();
		}
		PORTB |= (1<<6);
		fprintf_P(stderr,PSTR("\nTWCR = ")); 
		print_bin(TWCR,stderr);		
		PORTB&=(uint8_t)~(1<<6);
		_delay_ms(500);
		
		PORTB |= (1<<6);
		fprintf_P(io_direct,PSTR("\nTWCR = ")); 
		print_bin(TWCR,io_direct);		
		PORTB&=(uint8_t)~(1<<6);
		
		_delay_ms(2000);
	}
	return 0;
}

