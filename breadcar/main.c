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
#include "usi-i2c.h"

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

int main(void){ 	
	init();
	
	for(;;) {
		if (update_heartbeat_led) {
			typedef enum {DN, UP} dir_t;
			#define LED_TOP_A 0x3ff
			#define LED_STEP_A 1
			static dir_t led_dir_A=UP;
			uint16_t LED_A;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				LED_A = OCR1A;
				LED_A += (TC1H<<8);
			}
			if (LED_A>(LED_TOP_A-LED_STEP_A))
				led_dir_A=DN;
			else if (LED_A<LED_STEP_A)
				led_dir_A=UP;
	
			if (led_dir_A==UP)
				LED_A+=LED_STEP_A;
			else LED_A-=LED_STEP_A;
	
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				TC1H = LED_A>>8;
				OCR1A = LED_A&0xff;
			}
		}
	}
	return 0;
}
