#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>

#include <util/atomic.h>

#include "timer.h"

static void timer0_init(void) { // 8, PWM
	fprintf_P(stderr,PSTR("\ntimers: init: timer0"));
	power_timer0_enable();

	// Diable Timer
	TCCR1B&= (uint8_t)~((1<<CS12)|(1<<CS11)|(1<<CS10));
	
	DDRB|=(1<<3)|(1<<4);
	
	// Pin ctrl
	//TCCR0A&=~((1<<COM0A1)|(1<<COM0A0)|(1<<COM0B1)|(1<<COM0B0));
	TCCR0A|=(1<<COM0A1)|(1<<COM0B1);TCCR0A&=(uint8_t)~((1<<COM0A0)|(1<<COM0B0));
	
	
	// Mode 2, CTC.
	//TCCR0B&=(uint8_t)~	(1<<WGM02);
	//TCCR0A|=		(1<<WGM01);	
	//TCCR0A&=(uint8_t)~	(1<<WGM00);
	
	// Mode 1, PWM.
	TCCR0B&=(uint8_t)~	(1<<WGM02);
	TCCR0A&=(uint8_t)~	(1<<WGM01);	
	TCCR0A|=		(1<<WGM00);
	
	TCNT0=0;
	OCR0A=0;
	OCR0B=0;
	
	// Interupts
	TIMSK0=(0<<OCIE0A)|(0<<OCIE0B);
	
	// Clock Select and timer enable {disable,1,8,64,256,1024,ExFalling,ExRising}
	TCCR0B|=1;
	
	
	fprintf_P(stderr,PSTR("\t[done]"));
}

static void timer1_init(void) { // 16, PWM
	fprintf_P(stderr,PSTR("\ntimers: init: timer1"));
	power_timer1_enable();

	// Disable Timer
	TCCR1B&= (uint8_t)~((1<<CS12)|(1<<CS11)|(1<<CS10));
	
	// 	 OC1A,	  OC1B set to outputs
	DDRD|= (1<<5)|(1<<4);
	
	// Pin Control
	TCCR1A|= (1<<COM1A1)|(1<<COM1B1);
	TCCR1A&= (uint8_t)~((1<<COM1A0)|(1<<COM1B0));
	
	TCCR1B|= (1<<5); //Reserved bit
	
	// Mode 8.
	TCCR1B|= (1<<WGM13);
	TCCR1B&= (uint8_t)~(1<<WGM12);
	TCCR1A&= (uint8_t)~((1<<WGM11)|(1<<WGM10));
	
	// Mode 1.
	//TCCR1B&= (uint8_t)~((1<<WGM13)|(1<<WGM12));
	//TCCR1A&= (uint8_t)~(1<<WGM11);
	//TCCR1A|= (1<<WGM10);
	
	// Input noise canceler (irrelavent in pwm mode 8?)
	//TCCR1B&= (uint8_t)~(1<<ICNC1);
	
	// TOP (mode 8, mode 1 top = 0xFF)
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ICR1=0xFFFF;
		TCNT1=0;
		OCR1A=0;
		OCR1B=0;
	}
	
	// Interupts
	TIMSK1=(0<<ICIE1)|(0<<OCIE1B)|(0<<OCIE1A)|(0<<TOIE1);

	// Prescale and Enable.
	// 1
	//TCCR1B&= (uint8_t)~((1<<CS12)|(1<<CS11));
	TCCR1B|= (uint8_t) (1<<CS10);
	
	fprintf_P(stderr,PSTR("\t[done]"));
}


void timer2_init(void) { // 8, RTC
	fprintf_P(stderr,PSTR("\ntimers: init: timer2"));
	power_timer2_enable();

	// Disable Timer.
	TCCR2B&=(uint8_t)~((1<<CS22)|(1<<CS21)|(1<<CS20));
	
	// Disable Pin outputs
	TCCR2A&=(uint8_t)~((1<<COM2A1)|(1<<COM2A0)|(1<<COM2B1)|(1<<COM2B0));
	
	// Mode 2, CTC.
	TCCR2A|= (1<<WGM21);
	TCCR2A&=~(1<<WGM20);	
	TCCR2B&=~(1<<WGM22);

	// Enable OCR2A interupt
	TIMSK2=(1<<OCIE2A);
	
	TCNT2=0;
	OCR2B=0;
	
	// Clock Select
	//8000000/78/1024 == 100 HZ
	//8000000/125/64  == 1000 Hz
	#if	(T2HZ==100)	
	OCR2A=78;
	TCCR2B|=(1<<CS22)|(1<<CS21)|(1<<CS20); //1024
	#elif	(T2HZ==1000)
	OCR2A=125;
	TCCR2B|=(1<<CS22);TCCR2B&=(uint8_t)~((1<<CS21)|(1<<CS20));//64
	#else
	#error Invalid Timer2 Frequency
	#endif	

	fprintf_P(stderr,PSTR("\t[done]"));
}	
	
ISR(TIMER2_COMPA_vect) {
	static uint16_t sec;//=0
	static uint8_t ct;
	ct++;
	if (ct>=100) {
		ct=0;
		++sec;
		fprintf(stderr,PSTR("\n[debug] tick"));
	}
}


void timers_init(void) {
	fprintf_P(stderr,PSTR("\ntimers: init:\tstart"));
	
	//timer0_init(); //PWM 8
	//timer1_init(); //PWM 16
	timer2_init(); //RTC 8
	
	fprintf_P(stderr,PSTR("\ntimers: init:\t[done]"));
}
