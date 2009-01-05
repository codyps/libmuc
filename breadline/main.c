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

#include "motor.h"
#include "adc.h"
#include "usart.h"
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
	#else
		#error "F_CPU Unrecognized"
	#endif
	
	// Default
	//OSCCAL=0x84;

	// Posible
	//OSCCAL=0x8C;
}

void init(void) {
	power_all_disable();
	clock_init();
	MCUCR|=(1<<JTD); // Disable JTAG
	usart0_init();
	timers_init();
	adc_init();
	joy_init();
	sei();	
	printf_P(PSTR("\nmain: init:\t[done]\n\n"));	
}

void  print_bin(uint8_t inp) {
	for(int8_t j=7; j>=0; --j) {
	   	printf("%c",((inp&(1<<j))>>j)+'0');
	}
}

static char dirtoc(dir_t d) {
	if	(d==UP)
		return 'U';
	else if (d==DN)
		return 'D';
	else
		return '?';
}

ISR(BADISR_vect){
	fprintf_P(stderr,PSTR("\n\tError: Invalid Interupt\n"));
}

int main(void){ 	
	init();
	c_mode=WAIT;
	initial=true;
		
	for(;;) {
		if	(c_mode==FOLLOW) {
			printf_P(PSTR("\nmain: follow:\t[start]\n"));
			if (adc_data_new) {
				adc_data_new=false;
				if (initial) {
					initial=false;
					lf_speed_full();
				}
				
				// gen copy of fixed adc inputs.
				uint16_t adc_vc[ADC_CHANNEL_AMT];
				for (uint8_t i=0;i<ADC_CHANNEL_AMT;++i) {
					adc_vc[i]=adc_get_val(i);
				}
		
				uint8_t  max_i=ADC_CHANNEL_AMT;
				uint16_t max_v=0;
				for (uint8_t i=0;i<ADC_CHANNEL_AMT;++i) {
					if (adc_vc[i]>max_v) {
						max_i=i;
						max_v=adc_vc[i];
					}
				}
			
				#if DEBUG_L(1)
				print_adc_values();
				printf_P(PSTR("\nMax Chan [L 0 1 2 3 R]: %d;v=%d"),max_i,max_v);
				#endif

				int8_t max_i_S;			
				max_i_S = max_i-ADC_CHANNEL_AMT/2; 
				// Correction needed for even numbers of sensors.
				#if SENSOR_NUM_EVEN
				if (max_i_S>0)
					++max_i_S;	
				#endif

				//Find Sensor "next to" max sensor.
				uint8_t next_i=0;
				uint16_t next_v;
				if (max_i_S>0)
					next_i=max_i-1;
				else if (max_i_S<0)
					next_i=max_i+1;
				#if SENSOR_NUM_ODD // Odd sensor nums only
				else
					next_i=0; // should be 1 or -1, which ever is larger.
				#endif
				
				//Hack for lack of zero sensor on even sensored bots.
				#if SENSOR_NUM_EVEN
				if (next_i==0) {
					if (max_i_S<0)
						++next_i;
					else // max_i_S>0
						--next_i;
				}
				#endif	

				next_v=adc_vc[next_i];
				
				//TODO: Use next[vi], max[vi] to find a turn increment.
				// fst arg should be at most 256*2				
				lf_turn_inc((max_v-next_v)/(CHANNEL_AMT/2/abs(max_i_S)),SIGN(max_i_S));

				printf_P(PSTR("\nTurn Increment: %d, dir:%d"),max_v-next_v,SIGN(max_i_S));
			
				uint16_t cspeed [2] = {	motor_get_speed(LEFT ),\
							motor_get_speed(RIGHT)};
				printf_P(PSTR("\nCurr Motors: L:%3d %3d:R"),cspeed[0],cspeed[1]);
				
				
			}
			
			else { // if !new_adc_data
				// Sleep? (need adc, timers, pwm outputs (IO clock), 
				printf_P(PSTR("\nWaiting for adc data?"));
			}
			
		}
	}	
	return 0;
}
