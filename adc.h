#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "muc.h"
#include "adc_conf.h"

#define ADC_PRESCALE_BITS                     \
	( (uint8_t) ceil( log((double)F_CPU / \
		ADC_MAX_CLK)/log(2) ) )
#define ADC_PRESCALE \
	((uint8_t) pow(2,ADC_PRESCALE_BITS))
#define ADC_F (F_CPU/ADC_PRESCALE)
#define ADC_CYCLE ( F_CPU / ADC_F ) // == ADC_PRESCALE

#define ADC_CT ADC_CHANNEL_CT
#define ADC_CHANNEL_CT ARRAY_SIZE(adc_chan_map)

void adc_init(void);
uint16_t adc_get_i(uint8_t sensor_i);

void adc_val_cpy(uint16_t *dst);
extern uint16_t adc_values[ADC_CHANNEL_CT];
extern volatile bool adc_new_data;

#endif
