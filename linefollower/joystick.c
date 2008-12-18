#include <stdbool.h>
#include "defines.h"
#include "adc.h"
#include "joystick.h"
#include "motor.h"#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

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
			if (c_mode!=WAIT) {
				c_mode=WAIT;
				lf_stop_speed();
			}
			else {
				initial=true;
				c_mode=TEST;
			}
			#ifdef debug
			printf_P(PSTR("\nCurrent Mode (T/W) = %d"),c_mode);
			#endif
		}
		if (iPINE&(1<<3)) {// Right
			if (c_mode!=WAIT) {
				c_mode=WAIT;
				lf_stop_speed();
			}
			else {
				initial=true;
				c_mode=FOLLOW;
			}
			#ifdef debug	
			printf_P(PSTR("\nCurrent Mode (F/W)= %d"),c_mode);
			#endif
		}
	}
	#ifdef debug
	else
		printf_P(PSTR("\n[debug] PE? Released"));
	#endif
}

ISR(PCINT1_vect) {
	//PB7,4,6
	uint8_t iPINB = (uint8_t)~PINB;
	if (iPINB&((1<<7)|(1<<6)|(1<<4))) {
		if (iPINB&(1<<7)) {// Down
			adc_calibrate_update();
			#ifdef debug
			print_adc_calibration();
			print_adc_values();
			#endif
		}
		if (iPINB&(1<<4)) {// In
			#ifdef debug
			print_adc_calibration();
			print_adc_values();
			#endif
		}
		if (iPINB&(1<<6)) {// Up
			adc_calibrate_clear();
			#ifdef debug
			print_adc_calibration();
			print_adc_values();
			#endif
		}
	}
	#ifdef debug
	else
		printf_P(PSTR("\n[debug] PB? Released"));
	#endif
}
