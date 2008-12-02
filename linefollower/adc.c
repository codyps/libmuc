/*
Controls and Provides access to the Analog to Digital Converter (ADC) subsystem of the avr
*/

#include "defines.h"
#include "adc.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <stdio.h>
#include <math.h>
#include <util/delay_basic.h>
#include <avr/pgmspace.h>
#include <string.h>

uint8_t max(uint16_t val[],uint8_t sz) {
	uint8_t tmpi=sz,tmpv=0;
	for(uint8_t c=0;c<sz;++c)
		if (val[c]>tmpv) {
			tmpi=c;
			tmpv=val[c];
		}
	return tmpi;
}

void print_adc_values() {
	printf_P(PSTR("\n[debug]   RAW ADC: "));
	for (uint8_t chan=0;chan<channel_amt;chan++) 
		printf(" [ %d : %d ] ", chan, adc_val[chan]);
	
	printf_P(PSTR("\n[debug] FIXED ADC: "));
	for (uint8_t chan=0;chan<channel_amt;chan++) 
		printf(" [ %d : %d ] ", chan, adc_get_val(chan));
}

void print_adc_calibration() {
	printf_P(PSTR("\n[debug] ADC OFFSETS: "));
	for (uint8_t chan=0;chan<channel_amt;chan++) 
		printf(" [ %d : %d ] ", chan, adc_offset[chan]);
}


uint16_t adc_get_val(uint8_t ch) {
	return adc_val[ch]-adc_offset[ch];
}

static uint8_t num_calibrations;
void adc_calibrate_update() {
	uint16_t offsets[channel_amt];
	uint16_t adc_val_cpy[channel_amt];
	memcpy(adc_val_cpy,adc_val,channel_amt);
	
	
	
	uint8_t i = max(adc_val_cpy,channel_amt);
	
	for(uint8_t c=0;c<channel_amt;++c) {
		offsets[c]=adc_val_cpy[i]-adc_val_cpy[c];
	}
		
	if (num_calibrations==0)
		memcpy(adc_offset,offsets,channel_amt);
	else
		for(uint8_t c=0;c<channel_amt;++c) {
			adc_offset[c]=(adc_offset[c] * num_calibrations + offsets[c])/(num_calibrations+1);
		}
			
	++num_calibrations;
}

void adc_calibrate_store() {
	printf_P(PSTR("\n[debug] Warning: unimplimented function adc_calibrate_store called"));
}

void adc_calibrate_clear() {
	for(uint8_t j= 0;j<channel_amt;++j)
		adc_offset[j]=0;
	num_calibrations = 0;
}

void adc_init() {
	printf_P(PSTR("\nadc: init"));

	power_adc_enable();

	//Set Voltage to AVCC with external capacitor at AREF pin
	ADMUX|= (uint8_t)(1<<REFS0);
	ADMUX&=(uint8_t)~(1<<REFS1);
	//ADMUX&=~(1<<ADLAR); // Default disabled
	
	// Enable ADC, Inturupt, Trigger mode and set prescaler
	//ADCSRA=(((1<<ADEN)|(1<<ADIE)|(1<<ADATE))&0b11111000)|(ADC_PRESCALE);
	ADCSRA|= (uint8_t)(1<<ADEN)|(1<<ADIE)|(1<<ADATE);
	ADCSRA = (uint8_t)(ADCSRA & 0b11111000)|((uint8_t)ADC_PRESCALE);
	
	// Enable Free Running Mode 
	ADCSRB|= (1<<7); //reserved bit.
	ADCSRB&= (uint8_t)~(0b111); //(ADTS2:0)=0
	
	// Disable Digital reads from analog pins
	DIDR0 |= (uint8_t)((1<<ADC4D)|(1<<ADC5D)|(1<<ADC6D)|(1<<ADC7D));
	
	set_sleep_mode(SLEEP_MODE_ADC);
	printf_P(PSTR("\nadc: init: setup convertions"));
	adc_set_channel(curr_ch);
	//Start the convertions
	ADCSRA|= (1<<ADSC);

	// Wait one adc clock cycle and change the channel, done by interupt later.
	_delay_loop_2(ADC_CYCLE_DELAY);
	adc_set_channel(++curr_ch);
	
	// Wait for one set of convertions to complete.
	//_delay_loop_2(ADC_CYCLE_DELAY*26);

	printf_P(PSTR("\t[done]"));
}

void adc_set_channel(uint8_t channel) {
	//Prescale reset on each "trigger event"s
	//From Data Sheet: (Switching to Free Running mode (ADTS[2:0]=0) will)
	//(not cause a trigger event, even if the ADC Interrupt Flag is set.)
	ADMUX  = (uint8_t)((uint8_t)(ADMUX &0b11100000)|channels[channel]);
	//print_adc_values();
	//printf("V - chan %X\n",channel);
}

//ADC Interupt handler
ISR(ADC_vect) {
	// New conversion has already started.
	uint16_t adc_value;
	adc_value  =  ADCL;  
	adc_value += (ADCH<<8);
	uint8_t real_channel;

	// the curr_ch now has the chan of the on going conversion, we need the last one
	if (curr_ch==0)	real_channel = channel_amt-1; //curr_ch==0 
	else		real_channel = curr_ch-1;

	adc_val[real_channel] = adc_value;
	++adc_amt[real_channel];

	// Change the channel for the conversion after the one currently processing.
	if (++curr_ch >= channel_amt)	curr_ch = 0;
	
	//Wait for the clock to be set, 1 adc cycle.
	//_delay_loop2(ADC_CYCLE_DELAY);
	adc_set_channel(curr_ch);
	
	//printf("adc_value: %d",adc_value);
	
	//TODO: modify for running average (Ave= (Ave*(ct-1)+New)/ct)
	//INFO: Vin[V]=(ADCH·256+ADCL)·Vref[V]/1024
}
