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
#include "twi_i2c.h"
#include "i2c_HMC6343.h"

void init(void) {
	power_all_disable();
	usart1_init();
	twi_init();
	hmc6343_init_static();
	fprintf_P(io_init,PSTR("\n\n[main]: init done\n\n"));	
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
//	char * msg = "\nPwn the Spi bus.\n";
	
	i2c_start_xfer();
	for(;;) {		
			
		if (head_data_updated == true) {
			head_data_updated = false;
			printf_P(PSTR("\nhead:%d pitch:%d roll:%d\n"),\
				head.head,head.pitch,head.roll);
			_delay_ms(500);		
			i2c_start_xfer();
		}
		printf_P(PSTR("\nTWI State : %x") , TW_STATUS);
		_delay_ms(2000);
		i2c_reset_xfer();
	}
	return 0;
}

