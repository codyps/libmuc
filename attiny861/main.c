/*
  attiny 861
 */

#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include <util/atomic.h>

#include "pwm.h"
#include "i2cmaster.h" // asm
#include "i2c_dev_HMC6343.h"
#include "softuart.h"

void clock_init(void) {
	
	#if   (F_CPU == 1000000)
		clock_prescale_set(clock_div_8);	
	#elif (F_CPU == 2000000)
		clock_prescale_set(clock_div_4);	
	#elif (F_CPU == 4000000)
		clock_prescale_set(clock_div_2);
	#elif (F_CPU == 8000000)
		clock_prescale_set(clock_div_1);
	#elif (F_CPU == 7833600) // For softuart (7.3728, 7.374848)
		#warning F_CPU=7.8336Mhz needs custom osccal
		clock_prescale_set(clock_div_1);
		OSCCAL=0xfa;//??
	#elif (F_CPU ==12800000)
		#warning F_CPU=12.8Mhz needs custom osccal
		clock_prescale_set(clock_div_1);
		//OSCCAL=0x7d or 7e + b9 to c8;
		OSCCAL=0x7e; 
	#elif (F_CPU ==14745600)
		#warning F_CPU=14.7456Mhz needs custom osccal
		clock_prescale_set(clock_div_1);
		//OSCCAL=0xd7 to e4;
		OSCCAL=0xe8;
	#elif (F_CPU ==18432000)
		#warning F_CPU=18.4320Mhz needs custom osccal (and is way out of spec)
		clock_prescale_set(clock_div_1);
		//OSCCAL=0xf7 to fe;
		OSCCAL=0xfa;
	#elif (F_CPU ==16000000)
		#warning F_CPU=16Mhz untested	
	#else
		#error "F_CPU Unrecognized"
	#endif
}

void init(void) {
	//power_all_disable();
	clock_init();
	softuart_init();
	pwm_init();
	i2c_init(); //asm lib
	sei();
	DEBUG_LED_DDR|=(1<<DEBUG_LED_POS)|(1<<ERROR_LED_POS);
	DEBUG_LED_FLIP;
	softuart_stderr();
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
	
	#if defined(OSCCAL_CALIBRATE)
	for(;;) {
		fprintf(stderr,"OSCCAL: %x ; CALC_CPU: %lu\n", OSCCAL, (unsigned long)SOFTUART_CPU);
		OSCCAL++;
		ERROR_LED_FLIP;
		_delay_ms(500);
		ERROR_LED_FLIP;
	}
	#endif	

	fprintf(stderr,"\nstarting i2c polling");
	update_head_i2c=true;
	for(;;) {
		if (update_head_i2c) {	
			DEBUG_LED_FLIP;		
			fprintf(stderr,"\nAttempt to read heading...");
			//Head[2], Pitch[2], Roll[2]
			struct {
				union {
					uint16_t head;
					struct {
					uint8_t head_lsb;
					uint8_t head_msb;
					};
				};
				union {
					int16_t pitch;
					struct {
					uint8_t pitch_lsb;
					uint8_t pitch_msb;
					};
				};
				union {
					int16_t roll;
					struct {
					uint8_t roll_lsb;
					uint8_t roll_msb;
					};
				};
			} heading_data;

			i2c_start_wait(HMC6343_ADDR_W);
    			i2c_write(HMC6343_POST_HEAD); 
                         
			i2c_rep_start(HMC6343_ADDR_R);
			heading_data.head_msb	=i2c_read(1);
			heading_data.head_lsb	=i2c_read(1);
			heading_data.pitch_msb	=i2c_read(1);			
			heading_data.pitch_lsb	=i2c_read(1);
			heading_data.roll_msb	=i2c_read(1);
			heading_data.roll_lsb	=i2c_read(0);

			i2c_stop();

			fprintf_P(stderr, PSTR("bearing:  head:%d  pitch:%d  roll:%d \n"),\
				 heading_data.head,heading_data.pitch,heading_data.roll);
			DEBUG_LED_FLIP;
		}
		
	}
	return 0;
}
