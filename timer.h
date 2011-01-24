#ifndef TIMER_H_
#define TIMER_H_

/** Timer1, ATMEGA328P **/
/* Table 15-4. Waveform Generation Mode Bit Description
 * n  WGM13:0 Desc                        TOP    OCR1x_up. TOV1_flag_set
 * 0  0 0 0 0 Normal                      0xFFFF Immediate MAX
 * 1  0 0 0 1 PWM, Phase Correct, 8-bit   0x00FF TOP       BOTTOM
 * 2  0 0 1 0 PWM, Phase Correct, 9-bit   0x01FF TOP       BOTTOM
 * 3  0 0 1 1 PWM, Phase Correct, 10-bit  0x03FF TOP       BOTTOM
 * 4  0 1 0 0 CTC                         OCR1A  Immediate MAX
 * 5  0 1 0 1 Fast PWM, 8-bit             0x00FF BOTTOM    TOP
 * 6  0 1 1 0 Fast PWM, 9-bit             0x01FF BOTTOM    TOP
 * 7  0 1 1 1 Fast PWM, 10-bit            0x03FF BOTTOM    TOP
 * 8  1 0 0 0 PWM, Phase and Freq Correct ICR1   BOTTOM    BOTTOM
 * 9  1 0 0 1 PWM, Phase and Freq Correct OCR1A  BOTTOM    BOTTOM
 * 10 1 0 1 0 PWM, Phase Correct          ICR1   TOP       BOTTOM
 * 11 1 0 1 1 PWM, Phase Correct          OCR1A  TOP       BOTTOM
 * 12 1 1 0 0 CTC                         ICR1   Immediate MAX
 * 13 1 1 0 1 (Reserved)                  –      –         –
 * 14 1 1 1 0 Fast PWM                    ICR1   BOTTOM    TOP
 * 15 1 1 1 1 Fast PWM                    OCR1A  BOTTOM    TOP
 */

#include <avr/io.h>
#include <avr/power.h>

#define TIMER1_INIT_PWM(t1_top) do {					\
	power_timer1_enable();						\
	/* Stop timer */						\
	TCCR1B = 0;							\
	/* Enable both compare match outputs. (COM1XY) */		\
	TCCR1A = (1 << COM1A1) | (0 << COM1A0)				\
		|(1 << COM1B1) | (0 << COM1B0)				\
		|(0 << WGM11 ) | (0 << WGM10 );				\
	/* reset counts and compare matches */				\
	TCNT1 = 0;							\
	OCR1A = 0;							\
	OCR1B = 0;							\
	ICR1 = (t1_top); /* TOP */					\
	/* no interrupts for us */					\
	TIMSK1 = (0 << ICIE1) | (0 << OCIE1B) | (0 << OCIE1A)		\
		|(0 << TOIE1);						\
	/* Clear all the interrupt flags */				\
	TIFR1 = (1 << ICF1) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1);\
	/* Set waveform (again) and enable (prescaler bits != 0)	\
	 * WGM13:2 = 10 */						\
	TCCR1B = (0 << ICNC1) | (0 << ICES1) | (1 << WGM13)		\
		|(0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);\
} while(0)

/** Timer 0, atmega328p **/
/* Table 14-8. Waveform Generation Mode Bit Description
n WGM0[2:0] Desc		TOP	update_OCRx_at	TOV_flag_set_on
0 0 0 0     Normal		0xFF	Immediate	MAX
1 0 0 1     PWM, Phase Correct	0xFF	TOP		BOTTOM
2 0 1 0     CTC			OCRA	Immediate	MAX
3 0 1 1     Fast PWM		0xFF	BOTTOM		MAX
4 1 0 0     Reserved		–	–		–
5 1 0 1     PWM, Phase Correct	OCRA	TOP		BOTTOM
6 1 1 0     Reserved		–	–		–
7 1 1 1     Fast PWM		OCRA	BOTTOM		TOP
Notes:
1. MAX = 0xFF
2. BOTTOM = 0x00
*/

#define TIMER0_INIT_PWM_MAX() do {					\
	power_timer0_enable();						\
	/* Stop timer */						\
	TCCR0B = 0;							\
	/* set on upcount, clear on down count. also set part of wave	\
	 * form */							\
	TCCR0A = (1 << COM0A1) | (0 << COM0A0)				\
		|(1 << COM0B1) | (0 << COM0B0)				\
		|(0 << WGM01 ) | (1 << WGM00);				\
	/* clear counts & compares */					\
	TCNT0 = 0;							\
	OCR0A = 0;							\
	OCR0B = 0;							\
	/* no interupts */						\
	TIMSK0 = 0;							\
	/* clear interupt flags */					\
	TIFR0 = (1 << OCF0B) | (1 << OCF0A) | (1 << TOV0);		\
	/* set last of waveform, prescale and enable */			\
	TCCR0B = (0 << WGM02)						\
		|(0 << CS02) | (0 << CS01) | (1 << CS00);		\
} while(0)

#endif
