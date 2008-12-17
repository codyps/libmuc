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
	   	printf("%c",((inp&(1<<j))>>j)+'0');
	}
}

void init(void) {
	cli();
	power_lcd_disable();
	power_spi_disable();
	clock_init();
	joy_init();
	usart_init();
	adc_init();
	timers_init();
	motors_init();
	sei();
	#ifdef debug
	printf_P(PSTR("\nInit: Done\n\n"));
	#endif
}



int main(void) {
	init();
	c_mode=WAIT;
	initial=true;
	
		
	for(;;) {
		if	(c_mode==FOLLOW) {	
			if (initial) {
				lf_full_speed();
				initial=false;
			}
//			uint16_t c_speed [2] = {motor_get_speed(LEFT),motor_get_speed(RIGHT)};

			uint16_t adc_vc[channel_amt];
			for (uint8_t i=0;i<channel_amt;++i) {
				adc_vc[i]=adc_get_val(i);
			}
			
			uint8_t  maxi=channel_amt;
			uint16_t maxv=0;
			for (uint8_t i=0;i<channel_amt;++i) {
				if (adc_vc[i]>maxv) {
					maxi=i;
					maxv=adc_vc[i];
				}
			}
			
			#ifdef debug
			print_adc_values();
			printf_P(PSTR("\nMax Channel [L 0 1 2 3 R]: %d;v=%d"),maxi,maxv);
			#endif

			int8_t turn_i;			
			turn_i = maxi-channel_amt/2; // Needs to work for even numbers....
			
			// Correction needed for even numbers of sensors.
			#if (!(channel_amt%2))
			if (turn_i>0)
				++turn_i;	
			#endif

			lf_turn_inc(abs(LF_INC_SMALL*turn_i),turn_i>=0);
			
			
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

			_delay_ms(200);
			// do at every adc calc or pwm vector.
		}
		else if	(c_mode==TEST) {
			if (initial) {
				motor_mode(MOTOR_L_FWD,LEFT);
				motor_mode(MOTOR_R_FWD,RIGHT);
				initial=false;
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
	printf_P(PSTR("\n\nInvalid Interupt Enabled\n"));
}

