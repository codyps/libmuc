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
	motor_mode(MOTOR_L_FWD,LEFT);
	motor_mode(MOTOR_R_FWD,RIGHT);
	sei();
	printf_P(PSTR("\nInit: Done\n\n"));
}



int main(void) {
	init();
	c_mode=WAIT;
	initial=true;
	
	motor_set_speed(0,LEFT);
	motor_set_speed(0,RIGHT);
		
	for(;;) {
		if		(c_mode==FOLLOW) {	
			if (initial) {
				lf_full_speed();
				printf("\nInitialize Speed.");
				initial=false;
			}
			uint16_t c_speed [2] = {motor_get_speed(LEFT),motor_get_speed(RIGHT)};
			printf("\nML: %X",c_speed[0]);
			printf("\nMR: %X",c_speed[1]);
			print_adc_values();
	
			uint16_t adc_val_mixed [2] = {	adc_get_val(0) + adc_get_val(1) * LF_ADC_MIX_WIEGHT,	\
											adc_get_val(3) + adc_get_val(2) * LF_ADC_MIX_WIEGHT	};

			if (adc_val_mixed[0]>adc_val_mixed[1])
				lf_turn_inc(LF_INC,-1);
			else if (adc_val_mixed[1]>adc_val_mixed[0])
				lf_turn_inc(LF_INC,1);
			else
				lf_full_speed();

			_delay_ms(700);
			// do at every adc calc or pwm vector.
		}
		else if	(c_mode==TEST) {
			uint16_t sp;
			
			motor_mode(MOTOR_L_FWD,LEFT);
			motor_mode(MOTOR_R_FWD,RIGHT);				
			
			printf_P(PSTR("\nMotor Speed L (max=%d):"),UINT16_MAX);
			scanf("%d",&sp);
			motor_set_speed(sp,LEFT);
			printf_P(PSTR("\nMotor Speed L is now %d"),motor_get_speed(LEFT));
			
			printf_P(PSTR("\nMotor Speed R (max=%d):"),UINT16_MAX);
			scanf("%x",&sp);
			motor_set_speed(sp,RIGHT);
			printf_P(PSTR("\nMotor Speed R is now %d"),motor_get_speed(RIGHT));
		
		}
	}	
} 
		
ISR(BADISR_vect) {
	printf_P(PSTR("\n\nInvalid Interupt Enabled\n"));
}

