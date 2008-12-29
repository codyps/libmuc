#include "defines.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include "timer.h"

void timer0_init(void) { // 8, PWM
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
	
	// Clock Select and timer enable
	//TCCR0B|=(1<<CS02)|(1<<CS00); TCCR0B&=(uint8_t)~(1<<CS01);		//1024
	TCCR0B|=(1<<CS02); TCCR0B&=(uint8_t)~((1<<CS01)|(1<<CS00));	//256
	//TCCR2B|=(1<<CS02);TCCR2B&=~((1<<CS01)|(1<<CS00));				//64
	
	
	fprintf_P(stderr,PSTR("\t[done]"));
}

void timer1_init(void) { // 16, PWM
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
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {ICR1=0xFFFF;}
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){TCNT1=0;}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){OCR1A=0;}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){OCR1B=0;}
	
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
	
	
	//ASSR&=~((1<<EXCLK)|(1<<AS2));
	
	TCNT2=0;
	OCR2B=0;
	#if	(T2HZ==1000)	
	OCR2A=125;
	#elif	(T2HZ==100)
	OCR2A=78;
	#endif
	
	// Clock Select
	//8000000/78/1024 == 100 HZ
	//8000000/125/64  == 1000 Hz
	#if	(T2HZ==100)	
	TCCR2B|=(1<<CS22)|(1<<CS21)|(1<<CS20); //1024
	#elif	(T2HZ==1000)
	TCCR2B|=(1<<CS22);TCCR2B&=(uint8_t)~((1<<CS21)|(1<<CS20));//64
	#endif	

	fprintf_P(stderr,PSTR("\t[done]"));
}	
	
ISR(TIMER2_COMPA_vect) {
	//timer.h: dir_t led_dir_A
	//timer.h: dir_t led_dir_B
	
	// Led A
	if (LED_A>(LED_TOP_A-LED_STEP_A))
		led_dir_A=DN;
	else if (LED_A<LED_STEP_A)
		led_dir_A=UP;
	
	if (led_dir_A==UP)
		LED_A+=LED_STEP_A;
	else LED_A-=LED_STEP_A;
	
	// Led B
	if (LED_B>(LED_TOP_B-LED_STEP_B))
		led_dir_B=DN;
	else if (LED_B<LED_STEP_B)
		led_dir_B=UP;
	
	if (led_dir_B==UP)
		LED_B+=LED_STEP_B;
	else LED_B-=LED_STEP_B;
	
	/*
	static uint8_t dwell_A=0;
	static uint8_t dwell_B=0;	
	
	if (dwell_A) {
		++dwell_A;
		if ((dwell_A>DWELL_TOP_A)&&(led_dir_A==UP)) {
			dwell_A=0;
			--OCR1A;
			led_dir_A=DN;
		}
		else if ((dwell_A>DWELL_BTM_A)&&(led_dir_A==DN)) {
			dwell_A=0;
			++OCR1A;
			led_dir_A=UP;
		}
	}
	else {
		if ((OCR1A==UINT16_MAX)||(OCR1A==0)) dwell_A=1;
		else
			if (led_dir_A==UP) 	++OCR1A;
			else				--OCR1A;
	}

	if (dwell_B) {
		++dwell_B;
		if ((dwell_B>DWELL_TOP_B)&&(led_dir_B==UP)) {
			dwell_B=0;
			--OCR1B;
			led_dir_A=DN;
		}
		else if ((dwell_B>DWELL_BTM_B)&&(led_dir_B==DN)) {
			dwell_B=0;
			++OCR1B;
			led_dir_A=UP;
		}
	}
	else {
		if ((OCR1B==UINT16_MAX)||(OCR1B==0)) dwell_B=1;
		else
			if (led_dir_B==UP) 	++OCR1B;
			else				--OCR1B;
	}
	*/
	
}


void timers_init(void) {
	fprintf_P(stderr,PSTR("\ntimers: init:\tstart"));
	
	timer0_init(); //PWM 8
	timer1_init(); //PWM 16
	timer2_init(); //RTC 8
	
	LED_A=0xFFFF; //OCR1A
	LED_B=0x0000; //OCR1B
	led_dir_A=UP;
	led_dir_B=DN;
	
	fprintf_P(stderr,PSTR("\ntimers: init:\t[done]"));
}
