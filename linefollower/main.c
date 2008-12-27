/*
//Follow the damn line
char Author [] ="Cody Schafer";
*/

#include <stdbool.h>
#include "defines.h"
#include "usart.h"
#include "adc.h"
#include "motor.h"
#include "timers.h"
#include "joystick.h"
#include <avr/power.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>


void clock_init(void) {
	
	#if   F_CPU == 1000000
		clock_prescale_set(clock_div_8);	
	#elif F_CPU == 2000000
		clock_prescale_set(clock_div_4);	
	#elif F_CPU == 4000000
		clock_prescale_set(clock_div_2);	
	#elif F_CPU == 8000000
		clock_prescale_set(clock_div_1);	
	#else
		#error "F_CPU Unrecognized"
	#endif
	
	//OSCAL set by the bootloader.
}

void  print_bin(uint8_t inp) {
	for(int8_t j=7; j>=0; --j) {
	   	fputc(((inp&(1<<j))>>j)+'0',stdout);
	}
}

void init(void) {
	cli(); // Starts with interrupts disabled?
	power_all_enable();
	PCMSK1=PCMSK0=EIMSK=0; // Butterfly doesn't remove it's interupts, block them.
	clock_init();
	joy_init();
	usart_init();
	adc_init();
	timers_init();
	motors_init();
	sei();
	#if (DEBUG)
	printf_P(PSTR("\nInit: Done\n\n"));
	#endif
}



int main(void) {
	init();
	c_mode=WAIT;
	initial=true;
		
	for(;;) {
		if	(c_mode==FOLLOW && new_adc_data) {
			if (new_adc_data) {
				new_adc_data=false;
				if (initial) {
					initial=false;
					lf_full_speed();
				}
				
				// gen copy of fixed adc inputs.
				uint16_t adc_vc[channel_amt];
				for (uint8_t i=0;i<channel_amt;++i) {
					adc_vc[i]=adc_get_val(i);
				}
		
				uint8_t  max_i=channel_amt;
				uint16_t max_v=0;
				for (uint8_t i=0;i<channel_amt;++i) {
					if (adc_vc[i]>max_v) {
						max_i=i;
						max_v=adc_vc[i];
					}
				}
			
				#ifdef debug
				print_adc_values();
				printf_P(PSTR("\nMax Chan [L 0 1 2 3 R]: %d;v=%d"),max_i,max_v);
				#endif

				int8_t max_i_S;			
				max_i_S = max_i-channel_amt/2; 
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
				lf_turn_inc(max_v-next_v,SIGN(max_i_S));				
	
				//lf_turn_inc(abs(LF_INC_SMALL*turn_i),turn_i>=0);


				printf_P(PSTR("\nTurn Increment: %d, dir:"),max_v-next_v,SIGN(max_i_S));
			
				uint16_t cspeed [2] = {	motor_get_speed(LEFT ),\
							motor_get_speed(RIGHT)};
				printf_P(PSTR("\nCurr Motors: L:%d %d:R"),cspeed[0],cspeed[1]);
				
				//0=LEFT, 3=RIGHT
				/*
				if		((adc_vc[0]>adc_vc[1])&&(adc_vc[0]>adc_vc[2])&&(adc_vc[0]>adc_vc[3])) {
					lf_turn_inc(LF_INC_LARGE,NEG);
					dir=LEFT;
				}
				else if ((adc_vc[3]>adc_vc[0])&&(adc_vc[3]>adc_vc[1])&&(adc_vc[3]>adc_vc[2])) {
					lf_turn_inc(LF_INC_LARGE,POS);
					dir=RIGHT;
				}
				else if	((adc_vc[2]>adc_vc[0])&&(adc_vc[2]>adc_vc[1])&&(adc_vc[2]>adc_vc[3])) {
					lf_turn_inc(LF_INC_SMALL,NEG);
					dir=LEFT;
				}
				else if ((adc_vc[1]>adc_vc[0])&&(adc_vc[1]>adc_vc[2])&&(adc_vc[1]>adc_vc[3])) {
					lf_turn_inc(LF_INC_LARGE,POS);
					dir=RIGHT;
				}
				else if ((adc_vc[0]<adc_vc[1])&&(adc_vc[2]>adc_vc[3])) {
					//lf_full_speed();
					dir=FWD;
				}
				else if ((adc_vc[0]==adc_vc[1])&&(adc_vc[1]==adc_vc[2])&&(adc_vc[2]==adc_vc[3])){
					//lf_full_speed();
					dir=FWD;
				}
				*/
			}
			/*
			else { // if !new_adc_data
				// Sleep? (need adc, timers, pwm outputs (IO clock), 
			}
			*/
		}
		else if	(c_mode==TEST) {
			if (initial) {
				initial=false;
				motor_mode(MOTOR_L_FWD,LEFT);
				motor_mode(MOTOR_R_FWD,RIGHT);
			}
			static uint16_t sp;
			
			motor_set_speed(sp,LEFT);
			motor_set_speed(sp,RIGHT);
			_delay_ms(2);
			//if (!(sp%0x100))
			//	printf("\nsp=%x",sp);
			++sp;
		}
	}	
} 
		
ISR(BADISR_vect) {
	fprintf_P(stderr,PSTR("\n\nInvalid Interupt Enabled\n"));
}

