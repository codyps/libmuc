/* 
 * File: timers.c
 * Function: control of timers and pwm outputs
 */

#include "adc.h"
#include "defines.h"
#include <util/atomic.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>



void timer2_init(void) {
	// External crystal : 32.768KHZ
	/*
	a. Disable the Timer/Counter2 interrupts by clearing OCIE2A and TOIE2.
	b. Select clock source by setting AS2 as appropriate.
	c. Write new values to TCNT2, OCR2A, and TCCR2A.
	d. To switch to asynchronous operation: Wait for TCN2UB, OCR2UB, and TCR2UB.
	e. Clear the Timer/Counter2 Interrupt Flags.
	f. Enable interrupts, if needed.
	*/
	
	// disable interupts (probably alread done)
	TIMSK2&=(uint8_t)~((1<<OCIE2A)|(1<<TOIE2));
	
	// Enable asyncronous clocking.
	ASSR&=~(uint8_t)(1<<EXCLK);
	ASSR|=(uint8_t)(1<<AS2);
	
	// Reset acculumator
	TCNT2 = 0;
	
	TCCR2A&=(uint8_t)~(1<<FOC2A);
	
	/* 	Mode	WGM21	WGM20	operation
		0	0		0		Normal
		1	0		1		PWM,Phase correct
		2	1		0		CTC
		3	1		1		Fast PWM
	*/
	// : CTC Mode.
	//TCCR2A|=(1<<WGM20);
	//TCCR2A&=~(1<<WGM21);
	// : Normal Mode.
	TCCR2A&=(uint8_t)~((1<<WGM20)|(1<<WGM21));
	
	/* Compare output mode (non-pwm) (OC2A)
		COM2A1	COM2A0	Result
		0		0		No output
		0		1		Toggle on compare match
		1		0		clear on compare match
		1		1		set on 		"		"
	*/
	// : No output
	TCCR2A&=(uint8_t)~((1<<COM2A1)|(1<<COM2A0));
	
	// Clock Select (Prescale) set

	//32768/1/256 = 128Hz
	//TCCR2A&=~((1<<CS22)|(1<<CS21));
	//TCCR2A|= (1<<CS20);
	
	// 32768/8/256 = 16Hz
	//TCCR2A|= (uint8_t)(1<<CS21);
	//TCCR2A&=(uint8_t)~((uint8_t)((1<<CS20)|(1<<CS22)));

	// 32768/128/256 = 1Hz
	TCCR2A|= (1<<CS20)|(1<<CS22);
	TCCR2A&=~(1<<CS21);

	// 32768/1024/256 = 1/8Hz
	//TCCR2A|= (1<<CS21)|(1<<CS20)|(1<<CS22);

	loop_until_bit_is_clear(ASSR,TCN2UB);
	loop_until_bit_is_clear(ASSR,OCR2UB);
	loop_until_bit_is_clear(ASSR,TCR2UB);
	
	// Enable overflow interrupt, disable match.
	TIMSK2|= (uint8_t)(1<<TOIE2);
	//TIMSK2&=~(1<<OCIE2A); // Disabled
}


// Timer2 Overflow
ISR(TIMER2_OVF_vect) {
	static uint16_t sec;//=0
	++sec;
	//1 Hz (16/16)
	printf("\tT: %ds\n",sec);
	//print_adc_values();
	//printf("Current Channel: %d\n", curr_ch);
}

void timer1_init(void) { // Runs the PWMs

	// Set OC1A on up, clear on down
	TCCR1A|= (uint8_t) (1<<COM1A1)|(1<<COM1A0);
	
	// Set OC1B on up, clear on down
	TCCR1A|= (uint8_t) (1<<COM1B1)|(1<<COM1B0);
	
	// Waveform Generation Set to PWM (Phase and frequency correct, mode 8)
	TCCR1A&= (uint8_t) ~((1<<WGM11)|(1<<WGM10));
	TCCR1B|= (uint8_t) (1<<5); //Reserved bit
	TCCR1B|= (uint8_t) (1<<WGM13);
	TCCR1B&= (uint8_t)~(1<<WGM12);
	
	// Disable Input noise canceler
	TCCR1B|= (uint8_t)(1<<ICNC1);
	
	// Set TOP
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ICR1=0xFFFF;
	}
	
	// Note: f[OCnxPFCPWM]=f[pwm]=f[clk_io]/(2*Prescale*TOP);
	//	Presacle = f[clk]/(2*f[pwm]*TOP)
	//	Max f[pwm] for TB6612FNG is 100KHz (100000HZ)
	//prescale = F_CPU/(2*100000*0xFFFF) = 
	// At 8MHz the best we get is 60Hz (bad. very bad.)
	
	// Set prescale to 1
	// TCCR1B = (TCCR1B & 0b11111000)|(log(prescale)/log(2));
	TCCR1B&= (uint8_t)~((1<<CS12)|(1<<CS11));
	TCCR1B|= (uint8_t) (1<<CS10);
	
	//OCR1A and OCR1B are the Compare / PWM registers
	//OCR1A = OCR1B = 0xFFFF; // Max = 65535,0xFFFF
	
	// disable the interupts (probably done by default).
	TIMSK1&= (uint8_t)~((1<<ICIE1)|(1<<OCIE1B)|(1<<OCIE1A)|(1<<TOIE1));
	
	DDRB|= (uint8_t)(1<<PB5)|(1<<PB6);
}
/* Timer/Counter1 Overflow */
//ISR(TIMER1_OVF_vect) {}

/* Timer/Counter Compare Match A */
//ISR(TIMER1_COMPA_vect) {}

/* Timer/Counter Compare Match B */
///ISR(TIMER1_COMPB_vect) {}


/*void timer0_init(void) {
}*/

void timers_init(void) {
//	timer0_init(); // Not implimented.
//	timer1_init(); //PWM
	timer2_init(); //RTC
}
