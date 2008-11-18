/*

*/

#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

#define ADC_MAX_CLK 200000
#define ADC_PRESCALE  ceil(log(F_CPU/ADC_MAX_CLK)/log(2))
// For F_CPU=2MHz,PRESCALE=4 gives clkadc of 125KHz

#define channel_amt 4
const static uint8_t channels [] = {4, 6, 5, 7}; //Actual channels we are using, indices are virtuals
uint16_t adc_val [channel_amt]; //Current Calculated Values (vchan+1=
uint16_t adc_amt [channel_amt];
uint8_t curr_ch; //=0 //Current 'Virtual' Channel

// Functions:

void print_adc_values(void);
void adc_init(void);
void adc_set_channel(uint8_t);

#endif
