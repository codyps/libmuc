/*
//Follow the damn line
Char [] Author=Cody Schafer;
*/

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
	PCMSK1&=~(1<<PCINT15);
	power_lcd_disable();
	power_spi_disable();
	clock_init();
	usart_init();
	timers_init();
	adc_init();
	//Motor Control Lines, set to outputs
	DDRD|=(uint8_t)((1<<M_AIN1)|(1<<M_AIN2)|(1<<M_BIN1)|(1<<M_BIN2));

	sei(); //We use interupts, so enable them.
	printf("init: done\n\n");
}

int main(void) {
	init();
//	set_motor_L(0);
//	set_motor_R(0);
	DDRB|=(1<<2)|(1<<3);
	motor_mode_L(MOTOR_L_FWD);
	motor_mode_R(MOTOR_R_FWD);
	PORTB|=(1<<2)|(1<<3);
	static unsigned int ct;
	for (;;) {
		++ct;
		//ct = 1 for first iter
		if (ct%2){	// 1st
			PORTB^=(1<<2)|(1<<3);
		}
		

	//	printf("ML: %X\n",get_motor_L());
	//	printf("MR: %X\n",get_motor_R());
		printf("       76543210\n");
		printf("PORTB: ");print_bin(PORTB);printf("\n");
		printf("PORTD: ");print_bin(PORTD);printf("\n");
		_delay_ms(700);
	}	
} 
