/* AVR BUTTERFLY
 * File: timers.c
 * Function: control of timers and pwm outputs
 */

#include "adc.h"
#include "defines.h"
#include <util/atomic.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/power.h>


void timer2_init(void) {
	#if DEBUG_L(1)
	fprintf_P(stderr,PSTR("\ntimers: init: timer2"));
	#endif
	//power_timer2_enable(); // can't be disabled in m169p
	
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
	ASSR&=(uint8_t)~(1<<EXCLK);
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
	TCCR2A&=(uint8_t)~(1<<CS21);

	// 32768/1024/256 = 1/8Hz
	//TCCR2A|= (1<<CS21)|(1<<CS20)|(1<<CS22);

	loop_until_bit_is_clear(ASSR,TCN2UB);
	loop_until_bit_is_clear(ASSR,OCR2UB);
	loop_until_bit_is_clear(ASSR,TCR2UB);
	
	// Enable overflow interrupt, disable match.
	//TIMSK2|= (1<<TOIE2);
	//TIMSK2&=~(1<<OCIE2A); // Disabled
	#if DEBUG_L(1)
	fprintf_P(stderr,PSTR("\t[done]"));
	#endif
}


// Timer2 Overflow
ISR(TIMER2_OVF_vect) {
	static uint16_t sec;//=0
	++sec;
	if (c_mode==WAIT) {
		//printf("\n\tT: %ds\n",sec);
		#if DEBUG_L(1)
		fprintf_P(stderr,PSTR("\nMode: %d"),c_mode);
		#endif
	}

}

void timer1_init(void) { // Runs the PWMs
	#if DEBUG_L(1)
	fprintf_P(stderr,PSTR("\ntimers: init: timer1"));
	#endif
	power_timer1_enable();
	// Set OC1A/B on up, clear on down
//	TCCR1A|= (1<<COM1A1)|(1<<COM1A0);
//	TCCR1A|= (1<<COM1B1)|(1<<COM1B0);

	// OC1A/B disconnected
	TCCR1A&= (uint8_t) ~((1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1)|(1<<COM1B0));
	
	// Waveform Generation Set to PWM (Phase and frequency correct, mode 8)
	//TCCR1A&= (uint8_t) ~((1<<WGM11)|(1<<WGM10));
	
	// Phase correct only
	TCCR1A|= (1<<WGM11);
	TCCR1A&= (uint8_t) ~(1<<WGM11);
	
	TCCR1B|= (uint8_t) (1<<5); //Reserved bit
	TCCR1B|= (uint8_t) (1<<WGM13);
	TCCR1B&= (uint8_t)~(1<<WGM12);

	// Disable Input noise canceler
	TCCR1B&= (uint8_t)~(1<<ICNC1);
	
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
	
	// Enable the interupts, used to avoid the fucking speaker.
	TIMSK1|=((1<<OCIE1B)|(1<<OCIE1A)|(1<<TOIE1));
	
	MOTOR_PWM_DDR|= ((1<<M_PWMA_PIN)|(1<<M_PWMB_PIN));
	#if DEBUG_L(1)
	fprintf_P(stderr,PSTR("\t[done]"));
	#endif
}

enum {DOWN, UP};
static uint8_t timer_2_dir;
/* Timer/Counter1 Overflow ; BOTTOM */
ISR(TIMER1_OVF_vect) {
	timer_2_dir=UP;
}

/* Timer/Counter1 ISR1 ; TOP */
ISR(SIG_INPUT_CAPTURE1) {
	timer_2_dir=DOWN;
}

static char dirtoc(uint8_t dir){
	if (dir==UP)
		return 'U';
	else if (dir==DOWN)
		return 'D';
	else
		return '?';
}


/* Timer/Counter Compare Match A */
ISR(TIMER1_COMPA_vect) {
	if (timer_2_dir==UP)	
		MOTOR_PWM_PORT&=(uint8_t)~(1<<M_PWMA_PIN);
	else
		MOTOR_PWM_PORT|=(1<<M_PWMA_PIN);
	//debugpf("\nCA:%c:%x",dirtoc(timer_2_dir),TCNT1);	
}

/* Timer/Counter Compare Match B */
ISR(TIMER1_COMPB_vect) {
	if (timer_2_dir==UP)	
		MOTOR_PWM_PORT&=(uint8_t)~(1<<M_PWMB_PIN);
	else
		MOTOR_PWM_PORT|=(1<<M_PWMB_PIN);
	//debugpf("\nCB:%c:%x",dirtoc(timer_2_dir),TCNT1);
}


/*
void timer0_init(void) {}
*/

void timers_init(void) {
	fprintf_P(stderr,PSTR("\ntimers: init: start."));
//	timer0_init(); // Not implimented.
	timer1_init(); //PWM
	timer2_init(); //RTC
	fprintf_P(stderr,PSTR("\ntimers: init:\t[done]"));
	
}
