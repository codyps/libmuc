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
		if (val[c]<tmpv) {
			tmpi=c;
			tmpv=val[c];
		}
	return tmpi;
}

void print_adc_values() {
	fprintf_P(stderr,PSTR("\n[debug]   RAW ADC: "));
	for (uint8_t chan=0;chan<CHANNEL_AMT;chan++) 
		fprintf(stderr," [ %d : %d ] ", chan, adc_val[chan]);
	
	fprintf_P(stderr,PSTR("\n[debug] FIXED ADC: "));
	for (uint8_t chan=0;chan<CHANNEL_AMT;chan++) 
		fprintf(stderr," [ %d : %d ] ", chan, adc_get_val(chan));
}

void print_adc_calibration() {
	fprintf_P(stderr,PSTR("\n[debug] ADC OFFSETS: "));
	for (uint8_t chan=0;chan<CHANNEL_AMT;chan++) 
		fprintf(stderr," [ %d : %d ] ", chan, adc_offset[chan]);
}


uint16_t adc_get_val(uint8_t ch) {
	return adc_val[ch]+adc_offset[ch];
}

static uint8_t num_calibrations;
void adc_calibrate_update() {
	int16_t offsets[CHANNEL_AMT];
	uint16_t adc_val_cpy[CHANNEL_AMT];
	memcpy(adc_val_cpy,adc_val,sizeof(adc_val));
	
	
	uint8_t i=0,val=0;
	for(uint8_t c=0;c<CHANNEL_AMT;++c) {
		if(val>adc_val_cpy[c]) {
			i=c;
			val=adc_val_cpy[c];
		}
	}
	
	for(uint8_t c=0;c<CHANNEL_AMT;++c) {
		offsets[c]=adc_val_cpy[i]-adc_val_cpy[c];
	}
		
	if (num_calibrations==0)
		memcpy(adc_offset,offsets,sizeof(adc_offset));
	else
		for(uint8_t c=0;c<CHANNEL_AMT;++c) {
			adc_offset[c]=(adc_offset[c] * num_calibrations + offsets[c])/(num_calibrations+1);
		}
			
	++num_calibrations;
}

void adc_calibrate_store() {
	fprintf_P(stderr,PSTR("\n[debug] Warning: unimplimented function adc_calibrate_store called"));
}

void adc_calibrate_clear() {
	//for(uint8_t j= 0;j<CHANNEL_AMT;++j)
	//	adc_offset[j]=0;
	memset(adc_offset,0,sizeof(adc_offset));
	num_calibrations = 0;
}

void adc_init() {
	#if DEBUG_L(1)
	fprintf_P(stderr,PSTR("\nadc: init"));
	#endif
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
	#if DEBUG_L(3)
	fprintf_P(stderr,PSTR("\nadc: init: setup convertions"));
	#endif
	adc_set_channel(curr_ch);
	//Start the convertions
	ADCSRA|= (1<<ADSC);

	// Wait one adc clock cycle and change the channel, done by interupt later.
	_delay_loop_2(ADC_CYCLE_DELAY);
	adc_set_channel(++curr_ch);
	
	// Wait for one set of convertions to complete.
	//_delay_loop_2(ADC_CYCLE_DELAY*26);
	#if DEBUG_L(1)
	fprintf_P(stderr,PSTR("\t[done]"));
	#endif
}

void adc_set_channel(uint8_t channel) {
	//Prescale reset on each "trigger event"s
	//From Data Sheet: (Switching to Free Running mode (ADTS[2:0]=0) will)
	//(not cause a trigger event, even if the ADC Interrupt Flag is set.)
	ADMUX  = (uint8_t)((uint8_t)(ADMUX &0b11100000)|channels[channel]);
	//print_adc_values();
	//printf("V - chan %X\n",channel);
}

ISR(ADC_vect) {
	// Note: New conversion has already started.
	uint8_t real_channel;
	static uint8_t ct;
	uint16_t adc_value;
	adc_value  =  ADCL;  
	adc_value += (ADCH<<8);

	++ct;

	// the curr_ch now has the chan of the on going conversion, we need the last one
	if (curr_ch==0)	real_channel = CHANNEL_AMT - 1;
	else		real_channel = curr_ch-1;

	adc_val[real_channel] = adc_value;
	++adc_amt[real_channel];

	// Change the channel for the conversion after the one currently processing.
	if (++curr_ch >= CHANNEL_AMT)	curr_ch = 0;
	
	adc_set_channel(curr_ch);
	
	//printf("adc_value: %d",adc_value);
	
	//TODO: modify for running average (Ave= (Ave*(ct-1)+New)/ct)
	//INFO: Vin[V]=(ADCH·256+ADCL)·Vref[V]/1024
	
	if (!(ct%CHANNEL_AMT)) {
		//all values have been recalculated, update motors.
		adc_data_new=true;
	}
}
