#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>

#include "adc_conf.h"

void adc_init(void);
uint16_t adc_get_i(uint8_t sensor_i);

void adc_val_cpy(uint16_t *dst);
extern uint16_t adc_values[ADC_CHANNEL_CT];
extern volatile bool adc_new_data;

#endif
