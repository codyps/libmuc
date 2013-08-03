/* ADC: a simple API for fixed channel, continuous, interupted based analog to
 * digital converter processing for AVRs processing.
 */
#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>
#include <ccan/array_size/array_size.h>

/* adc_conf.h should include:
 * a definition of the desired ADC clock speed.

#define ADC_MAX_CLK KHz(200L)

 * a listing of the channels used.

static const uint8_t adc_chan_map [] = { 0, 4, 5, 6 };

 *
 */
#include "adc_conf.h"

#define ADC_CHANNEL_CT ARRAY_SIZE(adc_chan_map)

void adc_init(void);

/* use of adc_val_cpy() is recommended as it avoids repeated adc interrupt
 * disabling. */
uint16_t adc_get_i(uint8_t sensor_i);
void adc_val_cpy(uint16_t *dst);

/* set to 'true' when a full set of new data (a new reading for each sensor)
 * has been processed and is avaliable via adc_val_cpy() or adc_get_i() */
extern volatile bool adc_new_data;

#endif
