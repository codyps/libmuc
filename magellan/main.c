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
#include "timer.h"
#include "twi_i2c.h"
#include "i2c_HMC6343.h"

void clock_init(void) {
	
	#if   (F_CPU == 1000000)
		clock_prescale_set(clock_div_8);	
	#elif (F_CPU == 2000000)
		clock_prescale_set(clock_div_4);	
	#elif (F_CPU == 4000000)
		clock_prescale_set(clock_div_2);	
	#elif (F_CPU == 8000000)
		clock_prescale_set(clock_div_1);
	#else
		#error "F_CPU Unrecognized"
	#endif
}

void init(void) {
	power_all_disable();
	clock_init();
	MCUCR|=(1<<JTD); // Disable JTAG
	usarts_init();
	//timers_init();
	twi_init();
	hmc6343_init_static();
	sei();	
	printf_P(PSTR("\nmain: init:\t[done]\n\n"));	
}

void  print_bin(uint8_t inp) {
	for(int8_t j=7; j>=0; --j) {
	   	putc(((inp&(1<<j))>>j)+'0',stdout);
	}
}

ISR(BADISR_vect){
	fprintf_P(stderr,PSTR("\n\tError: Invalid Interupt\n"));
}

int main(void) { 	
	init();
	for(;;) {
		printf_P(PSTR("\nTWCR :     "));
		print_bin(TWCR);
		printf_P(PSTR("\nTW STATUS :"));
		print_bin(TW_STATUS);
		printf("   0x%x",TW_STATUS);
		if (head_data_updated == true) {
			head_data_updated = false;
			fprintf_P(stderr, PSTR("\nNew Data."));
			fprintf_P(stderr, PSTR("  head:%d  pitch:%d  roll:%d \n"),\
				head.head,head.pitch,head.roll);
		}
		_delay_ms(2000);
		i2c_start_xfer();
	}
	return 0;
}
