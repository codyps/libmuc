#include "defines.h"
#include "queue.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//char bits[128];
//uint8_t curr_bit;
queue_t data[3];


void card_reader_init(void) {
	// INT2 (CLK), Sample on downward edge.
	EICRA|=(1<<ISC21);
	EICRA&=(uint8_t)~(1<<ISC20);
	EIMSK|=(1<<INT2);
	
	// Motion and card detect pins.
	PCMSK2|=(1<<PCINT23)|(1<<PCINT22)|(1<<PCINT21);
	PCICR|=(1<<PCIE2);

	DDRB &=(uint8_t)~((1<<1)|(1<<2));
	PORTB|=((1<<1)|(1<<2));
	//PORTB&=(uint8_t)~((1<<1)|(1<<2));

	DDRC &=(uint8_t)~((1<<5)|(1<<6)|(1<<7));
	PORTC|=((1<<5)|(1<<6)|(1<<7));
	//PORTC&=(uint8_t)~((1<<5)|(1<<6)|(1<<7));
}



ISR(INT2_vect) {
	char bit = !((PINB&(1<<1))>>1)+'0';
	printf("%c",bit);
	//bits[curr_bit]=bit;
	//++curr_bit;
}

ISR(PCINT2_vect) {

	if (!(PINC&(1<<6)))	
		printf("\n");
	else if (!(PINC&(1<<7)))
		printf("\n");	
	/*
	data	= (PINB&(1<<1))>>1; //always high ?
	clk	= (PINB&(1<<2))>>2; //always high ?
	motion	= (PINC&(1<<5))>>5; //always low  ?
	fdet	= (PINC&(1<<7))>>7; //changes
	edet	= (PINC&(1<<6))>>6; //changes
	*/
	/*
	if (!(PINC&(1<<6)))
		printf("\n");
	else if (!(PINC&(1<<7)))
		printf("\n");		
	*/		 
}

