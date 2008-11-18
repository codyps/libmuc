#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "timer.h"
#include "defines.h"

enum {DN, UP};

void timer2_init(void) {
	// Disable Pin outputs
	TCCR2A&=~((1<<COM2A1)|(1<<COM2A0)|(1<<COM2B1)|(1<<COM2B0));
	
	// Mode 2, CTC.
	TCCR2A|=(1<<WGM21);
	TCCR2A&=~(1<<WGM20);	
	TCCR2B&=~(1<<WGM22);

	// Clock Select
	//8000000/78/1024 == 100 HZ
	//8000000/125/64  == 1000 Hz
	TCCR2B|=(1<<CS22)|(1<<CS21)|(1<<CS20);
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		OCR2A=78;
		OCR2B=0xFF;
	}

	// Enable OCR2A interupt
	TIMSK2|=(1<<OCIE2A);
}

/*
struct {
	uint8_t led_port;
	uint8_t led_pin;
	uint8_t led_dir;
	uint8_t dwell_btm;
	uint8_t dwell_top;
	uint8_t dwell;
	
} led_beat;
*/

ISR(TIMER2_COMPA_vect) {
	static uint8_t led_dir_A=UP;
	static uint8_t led_dir_B=DN;
	static uint8_t dwell_A=0;
	static uint8_t dwell_B=0;	
	
	if (dwell_A) {
		++dwell_A;
		if ((dwell_A>DWELL_TOP_A)&&(led_dir_A==UP)) {
			dwell_A=0;
			--OCR1A;
		}
		else if ((dwell_A>DWELL_BTM_A)&&(led_dir_A==DN)) {
			dwell_A=0;
			++OCR1A;
		}
	}
	else {
		if ((OCR1A==0xFFFF)||(OCR1A==0)) dwell_A=1;
		else
			if (led_dir_A==UP) 	++OCR1A;
			else			--OCR1A;
	}

	if (dwell_B) {
		++dwell_B;
		if ((dwell_B>DWELL_TOP_B)&&(led_dir_B==UP)) {
			dwell_B=0;
			--OCR1B;
		}
		else if ((dwell_B>DWELL_BTM_B)&&(led_dir_B==DN)) {
			dwell_B=0;
			++OCR1B;
		}
	}
	else {
		if ((OCR1B==0xFFFF)||(OCR1B==0)) dwell_B=1;
		else
			if (led_dir_B==UP) 	++OCR1B;
			else			--OCR1B;
	}
}

void timer1_init() {
	// Set to pwm p and f correct, pins set with ICR1 as top.

	// Clear pins on up count, set on down count. (COM1xn)
	// Mode 8.
	TCCR1A|= (1<<COM1A1)|(1<<COM1B1);
	TCCR1A&= (uint8_t)~((1<<COM1A0)|(1<<COM1B0)|(1<<WGM11)|(1<<WGM10));

	// Mode 8.
	TCCR1B|= (1<<5); //Reserved bit
	TCCR1B|= (1<<WGM13);
	TCCR1B&= (uint8_t)~(1<<WGM12);
	
	// Disable Input noise canceler
	TCCR1B|= (uint8_t)(1<<ICNC1);
	
	// Set TOP
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {ICR1=0xFFFF;}
	
	// Disable Interupts.
	TIMSK1&= (uint8_t)~((1<<ICIE1)|(1<<OCIE1B)|(1<<OCIE1A)|(1<<TOIE1));

	// Prescale = 1
	TCCR1B&= (uint8_t)~((1<<CS12)|(1<<CS11));
	TCCR1B|= (uint8_t) (1<<CS10);
	
	// 	 OC1A,	  OC1B set to outputs
	DDRB|= (1<<PD5)|(1<<PD4);
}

