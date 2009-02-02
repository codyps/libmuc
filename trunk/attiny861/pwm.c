#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "pwm.h"


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

	//	OC1A,  OC1B,  OC1D set to outputs
	DDRB|= (1<<1)|(1<<3)|(1<<5);  
	
	// Pin Control OC1{A,B,D} connected, !OC1{A,B,D} disconnected. PWM enabled
	TCCR1A= (1<<COM1A1)|(0<<COM1A0)|(1<<COM1B1)|(0<<COM1B0)\
		|(0<<FOC1A)|(0<<FOC1A)|(1<<PWM1A)|(1<<PWM1B);
	TCCR1C = (1<<COM1A1S)|(0<<COM1A0S)|(1<<COM1B1S)|(0<<COM1B0S)\
		|(1<<COM1D1)|(0<<COM1D0)|(0<<FOC1D)|(1<<PWM1D);
	
	// Mode
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
	//TIMSK|=(1<<TOIE1);

	

	// Prescale and Enable.
	// 64Mhz / 0x3FF / pow(2,Prescale-1) = 100kHz max
	// 64 000 000 / 1023 / pow(2,scaler-1) = 100 000
	// 64 000 000 / 1023 / 100 000 = pow(2,scaler-1)
	// log(64 000 000 / 1023 / 100 000) = (scaler-1)*log(2)
	// log(64 000 000 / 1023 / 100 000) / log(2) + 1 = scaler
	TCCR1B|=T1_PRESCALE;
	
	fprintf_P(stderr,PSTR("\t[done]"));
}

void pwm_set(volatile uint8_t * lowreg, uint16_t value) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		TC1H=value>>8;
		*lowreg=(uint8_t)value;
	}
}

void pwm_init(void) {
	fprintf_P(stderr,PSTR("\ntimers: init:\tstart"));
	
	//timer0_init(); // UART
	timer1_init(); // PWM 10
	
	pwm_set(&OCR1A,512);
	pwm_set(&OCR1B,512);
	pwm_set(&OCR1D,512);

	fprintf_P(stderr,PSTR("\ntimers: init:\t[done]"));
}
