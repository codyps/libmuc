/*
	ADC control for linefollowing.
*/

#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>
// Sensor connection information.
#define CHANNEL_AMT 4
const static uint8_t channels [] = {4, 6, 5, 7}; //Actual channels we are using, indices are virtuals

// ADC clock calculation
#define ADC_MAX_CLK 200000
#define ADC_PRESCALE  ceil(log(F_CPU/ADC_MAX_CLK)/log(2))
#define ADC_CYCLE F_CPU/F_CPU/pow(2,ADC_PRESCALE) // Possibly wrong too.
#define ADC_CYCLE_DELAY ADC_CYCLE/2  // I have a suspicion this is wrong
// F_CPU=2MHz,PRESCALE=4,clkadc=125kHz=125000
//		8Mhz,		6,			125kHz

// Autogened sensor information
#define SENSOR_NUM_ODD  (CHANNEL_AMT%2)
#define SENSOR_NUM_EVEN (!(CHANNEL_AMT%2))

// Various Data holds.
uint16_t volatile adc_val [CHANNEL_AMT]; //Current Calculated Values (vchan+1=
int16_t volatile adc_offset [CHANNEL_AMT]; //Offsets to apply to each channel (+-)
uint16_t volatile adc_amt [CHANNEL_AMT]; // Number of times a channel has been calculated.XXX: Not used
uint8_t volatile curr_ch; //=0 //Current 'Virtual' Channel
uint8_t volatile adc_data_new; // True when all the channels have been recalced.

// Functions:

uint16_t adc_get_val(uint8_t chan);
//#define adc_get_val(channel) adc_val[channel]+adc_offset[channel]

void adc_calibrate_update(void);
void adc_calibrate_store(void);
void adc_calibrate_clear(void);
void print_adc_calibration(void);
void print_adc_values(void);
void adc_init(void);
void adc_set_channel(uint8_t channel);

#endif
