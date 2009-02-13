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

#include "usart.h"
#include "twi_i2c.h"
#include "i2c_HMC6343.h"

void init(void) {
	power_all_disable();

	usarts_init();

	twi_init();
	hmc6343_init_static();

	sei();
	printf_P(PSTR("\n\n[main]: init done\n\n"));	
}

void  print_bin(uint8_t inp) {
	for(int8_t j=7; j>=0; --j) {
	   	putc(((inp&(1<<j))>>j)+'0',stdout);
	}
}

ISR(BADISR_vect){
	
	fprintf_P(stderr,PSTR("\n[error] bad isr\n"));
}

int main(void) { 	
	init();
//	char * msg = "\nPwn the Spi bus.\n";
	
	i2c_start_xfer();
	for(;;) {		
			
		if (head_data_updated == true) {
			head_data_updated = false;
			printf_P(PSTR("\n  head:%d  pitch:%d  roll:%d \n"),\
				head.head,head.pitch,head.roll);
			i2c_start_xfer();
		}
		i2c_start_xfer();
		printf_P(PSTR("\nTWI State : %x") , TW_STATUS);
		//spislave_put(msg,sizeof(msg) );
		_delay_ms(2000);		
	}
	return 0;
}

