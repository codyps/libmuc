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

void print_adc_values() {
	//printf("ADC: ");
	for (uint8_t chan=0;chan<channel_amt;chan++) 
		printf(" [ %d : %d ] ", chan, adc_val[chan]);
	printf("\n");
}

void adc_init() {
	printf("adc: init...\n");

	power_adc_enable();

	//Set Voltage to AVCC with external capacitor at AREF pin
	ADMUX|= (uint8_t)(1<<REFS0);
	ADMUX&=~(uint8_t)(1<<REFS1);
	//ADMUX&=~(1<<ADLAR); // Default disabled
	
	// Enable ADC, Inturupt, Trigger mode and set prescaler
	//ADCSRA=(((1<<ADEN)|(1<<ADIE)|(1<<ADATE))&0b11111000)|(ADC_PRESCALE);
	ADCSRA|= (uint8_t)(1<<ADEN)|(1<<ADIE)|(1<<ADATE);
	ADCSRA = (uint8_t)(ADCSRA & 0b11111000)|((uint8_t)ADC_PRESCALE);
	
	// Enable Free Running Mode 
	ADCSRB|= (uint8_t)(1<<7); //reserved bit.
	ADCSRB&= (uint8_t)~(0b111); //(ADTS2:0)=0
	
	// Disable Digital reads from analog pins
	DIDR0 |= (uint8_t)((1<<ADC4D)|(1<<ADC5D)|(1<<ADC6D)|(1<<ADC7D));
	
	set_sleep_mode(SLEEP_MODE_ADC);
	printf("adc: setup convertions...\n");
	adc_set_channel(curr_ch);
	//Start the convertions
	ADCSRA|= (uint8_t)(1<<ADSC);

	// Wait one adc clock cycle and change the channel
	// 125KHz
	//adc_set_channel(++curr_ch);
	// Right now it will compute the first sensor twice, and store it's first find in the 4th sensor.
	// Could use one of the timers to do this, but probably unesicary
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

	// the curr_ch now has the chan of the ongoing conversion, we need the last one
	if (!curr_ch)	real_channel = channel_amt-1; //curr_ch==0 
	else		real_channel = curr_ch-1;

	adc_val[real_channel] = adc_value;
	++adc_amt[real_channel];

	// Change the channel for the conversion after the one currently processing.
	if (++curr_ch >= channel_amt)	curr_ch = 0;
	
	adc_set_channel(curr_ch);
	
	//printf("adc_value: %d",adc_value);
	
	//TODO: modify for running average (Ave= (Ave*(ct-1)+New)/ct)
	//INFO: Vin[V]=(ADCH·256+ADCL)·Vref[V]/1024
}
