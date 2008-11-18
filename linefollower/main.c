/*
//Follow the damn line
char Author [] ="Cody Schafer";
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
	adc_init();	MOTOR_CTL_DDR|=(uint8_t)((1<<M_AIN1)|(1<<M_AIN2)|(1<<M_BIN1)|(1<<M_BIN2));
	motor_mode_L(MOTOR_L_FWD);
	motor_mode_R(MOTOR_R_FWD);
	sei(); //We use interupts, so enable them.
	printf("init: done\n\n");
}

int main(void) {
	init();
	set_motor_L(0);
	set_motor_R(0);
		
	//_delay_ms(500);
	//_delay_ms(500);	

			
	
	for (;;) {
		uint16_t c_speed [2] = {get_motor_L(),get_motor_R()};
		printf("ML: %X\n",c_speed[0]);
		printf("MR: %X\n",c_speed[1]);
		print_adc_values();
		
		uint16_t adc_val_mixed [2] = {	adc_val[0] + adc_val[1] * LF_ADC_MIX_WIEGHT,	\
						adc_val[3] + adc_val[2] * LF_ADC_MIX_WIEGHT	};

		if (adc_val_mixed[0]>adc_val_mixed[1])
			lf_turn_left_inc(LF_INC);
		else if (adc_val_mixed[1]>adc_val_mixed[0])
			lf_turn_right_inc(LF_INC);
		else
			lf_full_speed();

		
		//printf("       76543210\n");
		//printf("PORTB: ");print_bin(PORTB);printf("\n");
		//printf("PORTD: ");print_bin(PORTD);printf("\n");
		_delay_ms(700);
	}	
} 
