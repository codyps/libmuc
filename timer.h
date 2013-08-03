/*
 * Timer 0,1,2 setup for ATmega microcontrollers
 */

#ifndef TIMER_H_
#define TIMER_H_

/** Timer1, ATMEGA328P, ATMEGA8A **/
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

#define WGM1_NORMAL 0
#define WGM1_PWM_PC_8 1
#define WGM1_PWM_PC_9 2
#define WGM1_PWM_PC_10 3
#define WGM1_CTC_OCR1A 4
#define WGM1_FPWM_8 5
#define WGM1_FPWM_9 6
#define WGM1_FPWM_10 7
#define WGM1_PWM_PFC_ICR1 8
#define WGM1_PWM_PFC_OCR1A 9
#define WGM1_PWM_PC_ICR1 10
#define WGM1_PWM_PC_OCR1A 11
#define WGM1_CTC_ICR1 12
#define WGM1_RESERVED 13
#define WGM1_FPWM_ICR1 14
#define WGM1_FPWM_OCR1A 15

static inline uint8_t bi(uint8_t v, uint8_t i)
{
	return (v & (1 << i)) >> i;
}

static inline void timer1_stop(void)
{
	TCCR1B = 0;
}

static inline void timer1_reset(void)
{
	TCCR1B = 0;
	TIMSK = 0;
	TCNT1 = 0;
	OCR1A = 0;
	OCR1B = 0;
	ICR1 = 0;
	TCCR1A = 0;
#ifdef TIMSK
#if !defined(__AVR_ATmega8__)
#error "Ensure your part works, then add."
#endif
	/* TODO: mechanizm to combine this with timer{0,2}_reset() */
	TIMSK &= ~((1 << TICIE1) | (1 << OCIE1A) | (1 << OCIE1B) | (1 << TOIE1));
#elif defined(TIMSK1)
#error "Haven't tested since rewrite."
	TIMSK1 = 0;
#else
# error "Don't know how to disable interrupts."
#endif
}

static inline void timer1_start(uint16_t icr1, uint8_t w)
{
	/* Enable both compare match outputs. (COM1XY) */
	TCCR1A = (1 << COM1A1) | (0 << COM1A0)
		|(1 << COM1B1) | (0 << COM1B0)
		|(bi(w,1) << WGM11) | (bi(w,0) << WGM10);
	/* reset counts and compare matches */
	ICR1 = icr1; /* TOP */
	/* Set waveform (again) and enable (prescaler bits != 0)
	 * WGM13:2 = 10 */
	TCCR1B = (0 << ICNC1) | (0 << ICES1)
		|(bi(w,3) << WGM13) | (bi(w,2) << WGM12)
		|(0 << CS12) | (0 << CS11) | (1 << CS10);
}

#define TIMER1_INIT(icr1, w) do {					\
	power_timer1_enable();						\
	/* Stop timer */						\
	TCCR1B = 0;							\
	/* Enable both compare match outputs. (COM1XY) */		\
	TCCR1A = (1 << COM1A1) | (0 << COM1A0)				\
		|(1 << COM1B1) | (0 << COM1B0)				\
		|(bi(w,1) << WGM11) | (bi(w,0) << WGM10);		\
	/* reset counts and compare matches */				\
	TCNT1 = 0;							\
	OCR1A = 0;							\
	OCR1B = 0;							\
	ICR1 = (icr1); /* TOP */					\
	/* no interrupts for us */					\
	TIMSK1 = (0 << ICIE1) | (0 << OCIE1B) | (0 << OCIE1A)		\
		|(0 << TOIE1);						\
	/* Clear all the interrupt flags */				\
	TIFR1 = (1 << ICF1) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1);\
	/* Set waveform (again) and enable (prescaler bits != 0)	\
	 * WGM13:2 = 10 */						\
	TCCR1B = (0 << ICNC1) | (0 << ICES1)				\
		|(bi(w,3) << WGM13) | (bi(w,2) << WGM12)		\
		|(0 << CS12) | (0 << CS11) | (1 << CS10);		\
} while(0)

/** Timer 0, atmega328p **/
/* Table 14-8. Waveform Generation Mode Bit Description
 * n WGM0[2:0] Desc		TOP	update_OCRx_at	TOV_flag_set_on
 * 0 0 0 0     Normal		0xFF	Immediate	MAX
 * 1 0 0 1     PWM, Phase Correct	0xFF	TOP		BOTTOM
 * 2 0 1 0     CTC			OCRA	Immediate	MAX
 * 3 0 1 1     Fast PWM		0xFF	BOTTOM		MAX
 * 4 1 0 0     Reserved		–	–		–
 * 5 1 0 1     PWM, Phase Correct	OCRA	TOP		BOTTOM
 * 6 1 1 0     Reserved		–	–		–
 * 7 1 1 1     Fast PWM		OCRA	BOTTOM		TOP
 * Notes:
 * 1. MAX = 0xFF
 * 2. BOTTOM = 0x00
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

#define TIMER0_INIT_PERIODIC() do {					\
	power_timer0_enable();						\
	TCCR0B = 0;							\
	/* WGM = 0, COM* = 0 */						\
	TCCR0A = 0;							\
	TCNT0 = 0;							\
	OCR0A = 0;							\
	OCR0B = 0;							\
	TIMSK0 = 0;							\
	_Pragma(error, "not done")					\
} while(0)



/*** atmega328p, timer2.
 ** Table 17-8: Waveform Generation Mode Bit Description
 * n WGM[2:0]	Mode of Operation	TOP	Update of OCRx	TOV Flag set on
 * 0 0 0 0	Normal			0xFF	Immediate	MAX
 * 1 0 0 1	PWM, Phase Correct	0xFF	TOP		BOTTOM
 * 2 0 1 0	CTC			OCRA	Immediate	MAX
 * 3 0 1 1	Fast PWM		0xFF	BOTTOM		MAX
 * 4 1 0 0	Reserved		–	–		–
 * 5 1 0 1	PWM, Phase Correct	OCRA	TOP		BOTTOM
 * 6 1 1 0	Reserved		–	–		–
 * 7 1 1 1	Fast PWM		OCRA	BOTTOM		TOP
 * Notes:
 * 1. MAX= 0xFF
 * 2. BOTTOM= 0x00
 *
 ** Table 17-9.
 * CS2{2,1,0}
 *     0 0 0	No clock source (Timer/Counter stopped).
 *     0 0 1	clkT2S/1	(No prescaling)
 *     0 1 0	clkT2S/8	(From prescaler)
 *     0 1 1	clkT2S/32	(From prescaler)
 *     1 0 0	clkT2S/64	(From prescaler)
 *     1 0 1	clkT2S/128	(From prescaler)
 *     1 1 0	clkT2S/256	(From prescaler)
 *     1 1 1	clkT2S/1024	(From prescaler)
 */

#define TIMER2_PSC_1	1
#define TIMER2_PSC_8	2
#define TIMER2_PSC_32	3
#define TIMER2_PSC_64	4
#define TIMER2_PSC_128	5
#define TIMER2_PSC_256	6
#define TIMER2_PSC_1024	7

#define TIMER2_INIT_CTC(prescale, top) do {				\
	power_timer2_enable();						\
	TCCR2B = 0;							\
	TCNT2 = 0;							\
	OCR2A = top;							\
	OCR2B = 0;							\
	TIMSK2 = (0 << OCIE2B) | (1 << OCIE2A) | (0 << TOIE2);		\
	TIFR2  = (1 << OCF2B)  | (1 << OCF2A)  | (1 << TOV2);		\
	/* CTC mode */							\
	TCCR2A = (0 << WGM21) | (1 << WGM22);				\
	TCCR2B = (0 << WGM22) | (prescale & 0x7);			\
} while(0)

/*
 * Setup timer2 to run off an external 32.768kHz crystal and generate a
 * millisecond timer.
 *
 * Prescaller options:
 * 1, 8, 32, 64, 128, 256, 1024
 * Giving us clocks of:
 * 32.768, 4.096, 1.024, 0.512, 0.256, 0.128, 0.032
 *
 * And we have a 8bit counter allowing maximums of:
 * 128.0 16.0 4.0 2.0 1.0 0.5 0.125
 *
 * HW: atmega8a
 */
static inline void timer2_init_async_clock(void)
{
	// 1. Disable the Timer/Counter2 interrupts by clearing OCIE2 and
	//    TOIE2. (non-init only)
	TIMSK &= ~((1 << OCIE2) | (1 << TOIE2));

	// 2. Select clock source by setting AS2 as appropriate.
	ASSR = 1 << AS2;
	// TCNT2, OCR2, and TCCR2 might be corrupted by changing AS2.
	// Avoid setting up OC2 as an output prior to initializing this timer.

	// 3. Write new values to TCNT2, OCR2, and TCCR2.
	// - disable the counter
	TCCR2 = 0;
	loop_until_bit_is_clear(ASSR, TCR2UB);
	TCNT2 = 0;
	TCCR2 =  (0 << WGM20) | (0 << WGM21) // normal
		|(0 << COM21) | (0 << COM20) // diconnect OC2
		|(0 << CS22 ) | (0 << CS21 ) | (1 << CS20); // prescale = 1
	// OCR2 is unused

	// 4. To switch to asynchronous operation: Wait for TCN2UB, OCR2UB, and
	//    TCR2UB.
	loop_until_bit_is_clear(ASSR, TCR2UB);
	loop_until_bit_is_clear(ASSR, TCN2UB);

	// 5. Clear the Timer/Counter2 Interrupt Flags.
	TIFR = (1 << TOV2) | (1 << OCF2);

	// 6. Enable interrupts, if needed.
	TIMSK |= (1 << TOIE2);
}

#endif
