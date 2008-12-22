/*

*/

#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

#define ADC_MAX_CLK 200000
#define ADC_PRESCALE  ceil(log(F_CPU/ADC_MAX_CLK)/log(2))
#define ADC_CYCLE F_CPU/F_CPU/pow(2,ADC_PRESCALE)
#define ADC_CYCLE_DELAY ADC_CYCLE/2
// F_CPU=2MHz,PRESCALE=4,clkadc=125kHz=125000
//		8Mhz,		6,			125kHz

#define channel_amt 4
const static uint8_t channels [] = {4, 6, 5, 7}; //Actual channels we are using, indices are virtuals
uint16_t adc_val [channel_amt]; //Current Calculated Values (vchan+1=
int16_t adc_offset [channel_amt]; //Offsets to apply to each channel so they are equal.
uint16_t adc_amt [channel_amt];
uint8_t curr_ch; //=0 //Current 'Virtual' Channel

uint8_t new_adc_data; // True when all the channels have been recalced.

// Functions:
uint16_t adc_get_val(uint8_t ch);
void adc_calibrate_update(void);
void adc_calibrate_store(void);
void adc_calibrate_clear(void);
void print_adc_calibration(void);
void print_adc_values(void);
void adc_init(void);
void adc_set_channel(uint8_t);

#endif
