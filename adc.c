/*
 * ADC :
 *    single ended continuous processing.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>
#include <math.h>

#include "muc.h"
#include "adc.h"

#define ADC_PRESCALE_BITS                     \
	( (uint8_t) ceil( log((double)F_CPU / \
		ADC_MAX_CLK)/log(2) ) )
#define ADC_PRESCALE \
	((uint8_t) pow(2,ADC_PRESCALE_BITS))
#define ADC_F (F_CPU/ADC_PRESCALE)
#define ADC_CYCLE ( F_CPU / ADC_F ) // == ADC_PRESCALE

static uint16_t adc_values[ADC_CHANNEL_CT];
volatile bool adc_new_data;
static uint8_t adc_curr_chan_index;

static inline void adc_channel_set_from_index(uint8_t chan)
{
	/* 0xe0 == 0b11100000 */
	ADMUX = (ADMUX & 0xe0) | adc_chan_map[chan];
}

static inline void adc_channel_set_next(void)
{
	adc_curr_chan_index++;
	if (adc_curr_chan_index >= ADC_CHANNEL_CT)
		adc_curr_chan_index = 0;
	adc_channel_set_from_index(adc_curr_chan_index);
}

#define adc_isr_stop() do {		\
	ADCSRA &= ~(1 << ADIE);		\
	barrier();			\
} while(0)

#define adc_isr_start() do {		\
	ADCSRA |= (1 << ADIE);		\
	barrier();			\
} while(0)

void adc_val_cpy(uint16_t *dst)
{
	adc_isr_stop();
	memcpy(dst, adc_values, sizeof(adc_values));
	adc_isr_start();
}

uint16_t adc_get_i(uint8_t channel_index)
{
	uint16_t ret;
	adc_isr_stop();
	ret = adc_values[channel_index];
	adc_isr_start();
	return ret;
}

void adc_init(void)
{
	power_adc_enable();

	/* Digital Input Disable */
	uint8_t i;
	for (i = 0; i < ADC_CHANNEL_CT; i++) {
		uint8_t channel = adc_chan_map[i];
		/* XXX: hardware specific */
#if defined(__AVR_ATmega328P__)
		DIDR0 |= (1 << channel);
#else
# error "Hardware not supported by adc lib"
#endif

	}

#if defined(__AVR_ATmega644P__)
	/* REFS2 REFS1 REFS0 Voltage Reference (VREF) Selection
	 * ->  X     0     0   VCC , disconnected from AREF.
	 *     X     0     1   AREF pin, Internal Voltage Reference turned off.
	 *     0     1     0   Internal 1.1V Voltage Reference.
	 *     0     1     1   Reserved.
	 *     1     1     0   Internal 2.56V Voltage Reference w/o exterm
	 *                         bypass cap, disconnected from AREF.
	 *     1     1     1   Internal 2.56V Voltage Reference w/ extern
	 *                         bypass capacitor at AREF pin.
	 */
	ADMUX = (0<<REFS1) | (0<<REFS0)
		| (0<<ADLAR) /* Left adjust output when 1 */
		| (0<<MUX4) | (0<<MUX3) | (0<<MUX2) | (0<<MUX1) | (0<<MUX0);
#elif defined(__AVR_ATmega328P__)
	/* REFS1 REFS0 Voltage Reference Selection
	 *     0     0 AREF, Internal Vref turned off
	 * ->  0     1 AVCC with external capacitor at AREF pin
	 *     1     0 Reserved
	 *     1     1 Internal 1.1V Voltage Reference with external
	 *             capacitor at AREF pin
	 * NOTE: you had better have that capacitor.
	 */
	ADMUX = (0<<REFS1) | (1<<REFS0)
		| (0<<ADLAR) /* Left adjust output when 1 */
		| (0<<4) /* reserved */
		| (0<<MUX3) | (0<<MUX2) | (0<<MUX1) | (0<<MUX0);


#else
# error "unsupported avr"
#endif

	ADCSRA = (1<<ADEN)  /* Enable ADC */
		|(0<<ADSC)  /* Start Conversion	*/
		|(1<<ADATE) /* Enable auto trigger */
		|(1<<ADIF)  /* Interrupt flag */
		|(1<<ADIE)  /* Enable interrupt	*/
		|ADC_PRESCALE_BITS; /* Prescale, 3 bits */

#if defined(__AVR_ATmega644P__)
	/* ADTS2 ADTS1 ADTS0 Trigger Source
	 * ->  0     0     0   Free Running mode
	 *     0     0     1   Analog Comparator
	 *     0     1     0   External Interrupt Request 0
	 *     0     1     1   Timer/Counter0 Compare Match A
	 *     1     0     0   Timer/Counter0 Overflow
	 *     1     0     1   Timer/Counter0 Compare Match B
	 *     1     1     0   Timer/Counter1 Overflow
	 *     1     1     1   Watchdog Interrupt Request
	 */
	ADCSRB = (0<<BIN)   /* Bipolar input mode */
		|(0<<GSEL)  /* Diff input Gain, (20x -> 32x, 1x -> 8x) */
		|(0<<5)     /* Reserved, always read as 0 */
		|(0<<REFS2)
		|(0<<MUX5)
		|(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);
#elif defined(__AVR_ATmega328P__)
	/* ADTS2 ADTS1 ADTS0 Trigger Source
	 * 0 0 0 Free Running mode
	 * 0 0 1 Analog Comparator
	 * 0 1 0 External Interrupt Request 0
	 * 0 1 1 Timer/Counter0 Compare Match A
	 * 1 0 0 Timer/Counter0 Overflow
	 * 1 0 1 Timer/Counter1 Compare Match B
	 * 1 1 0 Timer/Counter1 Overflow
	 * 1 1 1 Timer/Counter1 Capture Event
	 */
	ADCSRB = (0 << ACME)
		|(0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0);
#else
# error "unsupported avr"
#endif

	adc_channel_set_from_index(adc_curr_chan_index);
	ADCSRA |= (1 << ADSC);

	/* wait one adc clock cycle before setting a new channel. */
	_delay_loop_2(ADC_PRESCALE);

	adc_channel_set_next();
}

ISR(ADC_vect)
{
	/* Note: New conversion has already started. */
	uint8_t past_channel_index;

	/* the curr_ch now has the chan of the on going conversion,
	 *    we want the previous value it held. */
	if (adc_curr_chan_index == 0)
		past_channel_index = ADC_CHANNEL_CT - 1;
	else
		past_channel_index = (uint8_t) (adc_curr_chan_index - 1);

	adc_values[past_channel_index] = ADC;

	if (adc_curr_chan_index == 0)
		adc_new_data = true;

        /* Needs ADC_PRESCALE clocks (40 on 8Mhz) from the interupt to the ADMUX
	 *   write within this function. */
	adc_channel_set_next();
}
