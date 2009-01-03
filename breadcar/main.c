/*
  attiny 861
 */

#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "timer.h"

void clock_init(void) {
	
	#if   (F_CPU == 1000000)
		clock_prescale_set(clock_div_8);	
	#elif (F_CPU == 2000000)
		clock_prescale_set(clock_div_4);	
	#elif (F_CPU == 4000000)
		clock_prescale_set(clock_div_2);	
	#elif (F_CPU == 8000000)
		clock_prescale_set(clock_div_1);
	#elif (F_CPU == 16000000)
		#warning F_CPU=16Mhz untested	
	#else
		#error "F_CPU Unrecognized"
	#endif
}

void init(void) {
	//power_all_disable();
	clock_init();
	timers_init();
	sei();
	DEBUG_LED_DDR|=(1<<DEBUG_LED_POS)|(1<<ERROR_LED_POS);
	DEBUG_LED_FLIP;
	fprintf_P(stderr,PSTR("\nmain: init:\t[done]\n\n"));	
}

static void  print_bin(uint8_t inp,FILE* stream) {
	for(int8_t j=7; j>=0; --j) {
	   	putc(((inp&(1<<j))>>j)+'0',stream);
	}
}

ISR(BADISR_vect){
	ERROR_LED_FLIP;
}

void init_usi_i2c(void) {
/*
		     USI Data Register Clock
USICS1 USICS0 USICLK Source                  4-bit Counter Clock Source
   0      0      0   No Clock                No Clock
                     Software clock strobe   Software clock strobe
   0      0      1
                     (USICLK)                (USICLK)
                     Timer/Counter0 Compare  Timer/Counter0 Compare
   0      1     X
                     Match                   Match
   1      0      0   External, positive edge External, both edges
   1      1      0   External, negative edge External, both edges
   1      0      1   External, positive edge Software clock strobe (USITC)
   1      1      1   External, negative edge Software clock strobe (USITC)
*/
	USICR = (1<<USISIE)|(1<<USIOIE)|\ // Interupts
		(1<<USIWM1)|(0<<USIWM0)|\ // Mode: 00=disable,01=3w,10=2w,11=2w(SCL held low)
		(0<<USICS1)|(1<<USICS0)|\ // Clock: 
		(0<<USICLK)|(0<<USITC)    // Clock Strobe; Toggle Clock

	// Use PA[2..0]
	USIPP|=(1<<USIPOS);
}

int main(void){ 	
	init();
	
	for(;;) {
		
		_delay_ms(100);

	}
	return 0;
}
