#include "defines.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <util/delay.h>
#include "timer.h"

static void timer0_init(void);
static void timer1_init(void);

static void timer0_init(void) { // 8,16 input capture
	fprintf_P(stderr,PSTR("\ntimers: init: timer0"));
	power_timer0_enable();

	// Diable Timer
	TCCR0B=(1<<TMS)|(1<<PSR0)|(0<<CS02)|(0<<CS01)|(0<<CS00);
	
	// CTC, input capture diabled
	TCCR0A=(0<<ICEN0)|(0<<TCW0)|(1<<WGM00)| \
		(0<<ICNC0)|(0<<ICES0)|(0<<ACIC0);
	
	// Interupts
	TIMSK|=(1<<OCIE0A);
	TIMSK&= (uint8_t) ~((1<<OCIE0B)|(1<<TOIE0)|(1<<TICIE0));

	/*
	FREQ: 8000000 / 64 / 125 = 1000
	FREQ: 8000000 / 64 / 200 = 625
	FREQ: 8000000 / 64 / 250 = 500
	FREQ: 8000000 / 256 / 25 = 1250
	FREQ: 8000000 / 256 / 50 = 625
	FREQ: 8000000 / 256 / 125 = 250
	FREQ: 8000000 / 256 / 250 = 125
	*/
	
	TCNT0H=0;
	TCNT0L=0;
	OCR0B=0;
	
	OCR0A=OCR0A_CTC_VAL;
	TCCR0B|=T0_PRESCALE_MSK;

	//Enable
	TCCR0B&=(uint8_t) ~(1<<TMS);

	fprintf_P(stderr,PSTR("\t[done]"));
}

static uint16_t ms;
static uint16_t sec;
ISR(TIMER1_COMPA_vect) {
	ms++;
	if (!(ms%1000)) { // Second
		++sec;		
	}
	if (!(sec%1000)) {
		DEBUG_LED_FLIP;
	}
}

static void timer1_init(void) { //PWM
/*	To change Timer/Counter1 to the asynchronous mode follow the procedure below:
    1. Enable PLL.
    2. Wait 100 μs for PLL to stabilize.
    3. Poll the PLOCK bit until it is set.
    4. Set the PCKE bit in the PLLCSR register which enables the asynchronous mode.
*/
	fprintf_P(stderr,PSTR("\ntimers: init: timer1"));
	power_timer1_enable();	
	
	// Disable Timer
	TCCR1B= (0<<PWM1X)|(1<<PSR1)|(0<<DTPS11)|(0<<DTPS10)\
		|(0<<CS13)|(0<<CS12)|(0<<CS11)|(0<<CS10);

	// Enable PLL
	PLLCSR=(LSM_MODE<<LSM)|(0<<PCKE)|(1<<PLLE);
	_delay_us(100);
	loop_until_bit_is_set(PLLCSR,PLOCK);
	PLLCSR|=(1<<PCKE);

	// 	 OC1A,	  OC1B set to outputs
	DDRB|= (1<<1)|(1<<3);  
	
	
	// Pin Control OC1{A,B} connected, !OC1{A,B,D} + OC1D disconnected.
	TCCR1A|= (1<<COM1A1)|(1<<COM1B1);
	TCCR1A&=(uint8_t)~((1<<COM1A0)|(1<<COM1B0));
	TCCR1B&=(uint8_t)~((1<<COM1D1)|(1<<COM1D1));

	// Mode
	TCCR1B&= (uint8_t)~((1<<PWM1X));
	TCCR1D = (0<<FPIE1)|(0<<FPEN1)|(0<<FPNC1)|(0<<FPES1)|(0<<FPAC1)\
		|(0<<WGM11)|(1<<WGM10);

	// TOP
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		TC1H=T1_TOP>>8;
		OCR1C=T1_TOP&0xFF; //TOP
		TC1H=0;
		TCNT1=0;
		OCR1A=0;
		OCR1B=0;
		OCR1D=0;
	}
	
	// Interupts
	TIMSK&=(uint8_t)~((1<<OCIE1D)|(1<<OCIE1A)|(1<<OCIE1B)|(1<<TOIE1));

	// Prescale and Enable.
	// 64Mhz / 0x3FF / pow(2,Prescale-1) = 100kHz max
	// 64 000 000 / 1023 / pow(2,scaler-1) = 100 000
	// 64 000 000 / 1023 / 100 000 = pow(2,scaler-1)
	// log(64 000 000 / 1023 / 100 000) = (scaler-1)*log(2)
	// log(64 000 000 / 1023 / 100 000) / log(2) + 1 = scaler
	TCCR1B|=T1_PRESCALE;
	
	fprintf_P(stderr,PSTR("\t[done]"));
}

void timers_init(void) {
	fprintf_P(stderr,PSTR("\ntimers: init:\tstart"));
	
	timer0_init(); // RTC 8 
	timer1_init(); // PWM 10
	
	fprintf_P(stderr,PSTR("\ntimers: init:\t[done]"));
}
