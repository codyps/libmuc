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

void joy_init(void) {
	// Set pins as inputs.
	DDRB&=(uint8_t)~((1<<4)|(1<<6)|(1<<7));
	DDRE&=(uint8_t)~((1<<2)|(1<<3));
	// Enable pullup resistors.
	PORTB|=((1<<4)|(1<<6)|(1<<7));
	PORTE|=((1<<2)|(1<<3));
	
	//DOWN	= PINB&(1<<7)
	//LEFT	= PINE&(1<<2)
	//RIGHT = PINE&(1<<3)
	//UP	= PINB&(1<<6)
	//IN	= PINB&(1<<4)
	
	// Enable Pin Change interupts. Disable INT0
	//EIMSK|=((1<<PCIE1)|(1<<PCIE0));
	//EIMSK&=(uint8_t)~(1<<INT0);
	EIMSK=(1<<PCIE1)|(1<<PCIE0);
	PCMSK1=(1<<PCINT15)|(1<<PCINT14)|(1<<PCINT12);
	PCMSK0=(1<<PCINT3)|(1<<PCINT2);	
	
}

ISR(PCINT0_vect) {
	//PE2,3
	uint8_t iPINE = (uint8_t)~PINE;
	if (iPINE&((1<<2)|(1<<3))) {
		if (iPINE&(1<<2)) {// Left
			if (c_mode!=WAIT)
				c_mode=WAIT;
			else {
				initial=true;
				c_mode=TEST;
			}
				
			printf_P(PSTR("\nCurrent Mode (T/W) = %d"),c_mode);
		}
		if (iPINE&(1<<3)) {// Right
			if (c_mode!=WAIT)
				c_mode=WAIT;
			else {
				initial=true;
				c_mode=FOLLOW;
			}
				
			printf_P(PSTR("\nCurrent Mode (F/W)= %d"),c_mode);
		}
	}
	else
		printf_P(PSTR("\n[debug] PE? Released"));
}

ISR(PCINT1_vect) {
	//PB7,4,6
	uint8_t iPINB = (uint8_t)~PINB;
	if (iPINB&((1<<7)|(1<<6)|(1<<4))) {
		if (iPINB&(1<<7)) {// Down
			adc_calibrate_update();
			print_adc_calibration();
			print_adc_values();
		}
		if (iPINB&(1<<4)) {// In
			print_adc_calibration();
			print_adc_values();
		}
		if (iPINB&(1<<6)) {// Up
			adc_calibrate_clear();
			print_adc_calibration();
			print_adc_values();
		}
	}
	else
		printf_P(PSTR("\n[debug] PB? Released"));
}

void init(void) {
	cli();
	power_lcd_disable();
	power_spi_disable();
	clock_init();
	joy_init();
	usart_init();
	adc_init();
	timers_init();	MOTOR_CTL_DDR|=((1<<M_AIN1)|(1<<M_AIN2)|(1<<M_BIN1)|(1<<M_BIN2));
	motor_set_speed(LF_MIN_SPEED,LEFT);
	motor_set_speed(LF_MIN_SPEED,RIGHT);
	motor_mode(MOTOR_L_FWD,LEFT);
	motor_mode(MOTOR_R_FWD,RIGHT);
	sei();
	printf_P(PSTR("\nInit: Done\n\n"));
}



int main(void) {
	init();
	c_mode=WAIT;
	initial=true;
	
	motor_set_speed(LF_MIN_SPEED,LEFT);
	motor_set_speed(LF_MIN_SPEED,RIGHT);
		
	for(;;) {
		if		(c_mode==FOLLOW) {	
			if (initial) {
				lf_full_speed();
			//	printf("\nInitialize Speed.");
				initial=false;
			}
//			uint16_t c_speed [2] = {motor_get_speed(LEFT),motor_get_speed(RIGHT)};
			//printf("\nML: %X",c_speed[0]);
			//printf("\nMR: %X",c_speed[1]);
			//print_adc_values();
			uint16_t adc_vc[4]={adc_get_val(0), adc_get_val(1),adc_get_val(2), adc_get_val(3)};
//			uint16_t adc_val_mixed [2] = {	adc_get_val(0) + adc_get_val(1) * LF_ADC_MIX_WIEGHT,	\
//											adc_get_val(3) + adc_get_val(2) * LF_ADC_MIX_WIEGHT	};
			
			static uint8_t old_dir=FWD;
			static uint8_t dir=FWD;
			static uint8_t ct;
			//0=LEFT, 3=RIGHT
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
			
			
			if (dir!=old_dir) {
				ct=0;
				//if (old_dir!=FWD)
				//	lf_full_speed();
			}
			else {
				++ct;
			}
			old_dir=dir;
			uint16_t integ = ct*LF_INC_INTEG;
			if (integ>LF_INTEG_MAX)
				integ=LF_INTEG_MAX;
			if (dir!=FWD)
				lf_turn_inc(integ,dir);
				
			//_delay_ms(5);
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

