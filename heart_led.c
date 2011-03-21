#include <avr/io.h>

/* PWM of LED */
static void timer0_init(void)
{
	/* OC0A clear on compare match, set at bottom
	 * OC0B disconected
	 */
	TCCR0A = (1 << COM0A1) | (0 << COM0A0)
		|(0 << COM0B1) | (0 << COM0B0)
}

/* Timer */
static void timer2_init(void)
{
	power_timer2_enable();

	/* Stop timer */
	TCCR2B = 0;

	OCR2A = 250;
	OCR2B = 0;
	TCNT2 = 0;

	/* Enable OC2A (top) interrupt for timer generation */
	TIMSK2 = (1 << OCIE2A);

	/* OC2A disconected
	 * OC2B clear on compare match, set at bottom
	 * Fast PWM (top = OCRA)
	 */
	TCCR2A = (0 << COM2A1) | (0 << COM2A0)
		|(1 << COM0B1) | (0 << COM0B0)
		|(1 << WGM21 ) | (1 << WGM20 );

	/* CS22 CS21 CS20 Description
	 * 0    0    0    No clock source (Timer/Counter stopped).
	 * 0    0    1    clkT2S/1    (No prescaling)
	 * 0    1    0    clkT2S/8    (From prescaler)
	 * 0    1    1    clkT2S/32   (From prescaler)
	 * 1    0    0    clkT2S/64   (From prescaler)
	 * 1    0    1    clkT2S/128  (From prescaler)
	 * 1    1    0    clkT2S/256  (From prescaler)
	 * 1    1    1    clkT2S/1024 (From prescaler)
	 */

	/* Fast PWM (top = OC2A)
	 * Clock selected; ps = 64
	 */
#define T2_CSPS_8  ((0 << CS22) | (1 << CS21) | (1 << CS20))
#define T2_CSPS_64 ((1 << CS22) | (0 << CS21) | (0 << CS20))
	TCCR2B = (1 << WGM22)
		| T2_CSPS_64;
}

static uint32_t ms_counter;
ISR(TIMER2_COMPA_vect)
{
	/* ms counter */
	ms_counter ++;
}

#define timer2_compa_isr_lock() do {      \
		TIMSK2 &= ~(1 << OCIE2A); \
		asm("":::"memory");       \
	} while(0)

#define timer2_compa_isr_unlock() do {    \
		TIMSK2 |= (1 << OCIE2A);  \
		asm("":::"memory");       \
	} while(0)

uint32_t time_ms(void) {
	uint32_t s;
	timer2_compa_isr_lock();
	s = ms_counter;
	timer2_compa_isr_unlock();
	return s;
}

void heart_init(void)
{
	timer2_init();
}

void heart_spin(void)
{

}

void heart_set_speed(uint8_t sp)
{

}

void heart_set_brightness(uint8_t br)
{

}
