/****************************************************************************
*
*   Copyright (c) 2008 www.societyofrobots.com
*   (please link back if you use this code!)
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation.
*
*   Alternatively, this software may be distributed under the terms of BSD
*   license.
*
****************************************************************************/

//AVR includes
#include <avr/io.h>		    // include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include <stdio.h>			// stuff
#include <stdlib.h>			// stuff
#include <math.h>			// stuff
//#include "libm.a"			// required with math.h
#include <string.h>			// allow strings to be used

//AVRlib includes
#include "global.h"		// include global settings
#include "uart4.h"		// include uart function library, includes buffer.h
#include "rprintf.h"	// include printf function library
//#include "timerx8.h"	// include timer function library (timing, PWM, etc)
#include "timer640.h"	// include timer function library (timing, PWM, etc)
#include "a2d.h"		// include A/D converter function library
#include "i2c.h"		// include i2c support
#include "spi.h"		// include spi support

//define port functions; example: PORT_ON( PORTD, 6);
#define PORT_ON( port_letter, number )			port_letter |= (1<<number)
#define PORT_OFF( port_letter, number )			port_letter &= ~(1<<number)
//#define PORT_ALL_ON( port_letter, number )		port_letter |= (number)
//#define PORT_ALL_OFF( port_letter, number )		port_letter &= ~(number)
#define FLIP_PORT( port_letter, number )		port_letter ^= (1<<number)
#define PORT_IS_ON( port_letter, number )		( port_letter & (1<<number) )
#define PORT_IS_OFF( port_letter, number )		!( port_letter & (1<<number) )

//define the servo function macro
#define servo(port,number,position)   (PORT_ON(port,number), delay_cycles(position), PORT_OFF(port,number))


//************CONFIGURE PORTS************
//configure ports for input or output - specific to ATmega2560
void configure_ports(void)
	{
	//examples: configure ports for input or output
	//DDRB = 0x1F;  //configure B ports 0->4 for output (google search '0b00011111 to hex')
	//DDRC = 0x7F;  //configure all C ports for output, except 7 (0b01111111 to hex)
	//DDRD = 0xFE;  //configure all D ports for output, except 0 and 1 (RX/TX) FC
    //DDRC = _BV (PC0); //PC0 is digital output
	//0b76543210 <- port # order, 1 for output, 0 for input
	//to convert from binary to hex, google search '0b00011111 to hex'
	//DDRG &= ~_BV(PG5); //make port G5 an input pin
	//cbi(DDRG, PG5);  //set G5 as input pin

	//ANALOG PORTS
	//useful for sensors, SharpIR, photoresistors, etc.
	//analog ports can be configured as digital ports if desired
	DDRF = 0b00000000;  //configure all F ports for input				0x00
	PORTF = 0b00000000; //make sure pull-up resistors are turned off	0x00
	DDRK = 0b00000000;  //configure all K ports for input				0x00
	PORTK = 0b00000000; //make sure pull-up resistors are turned off	0x00

	//DIGITAL PORTS
	//useful for servos, PWM, LED's, UART, interrupts, timers
	DDRA = 0b11111111;  //configure ports for output
	//       ||||||||
	//       |||||||\___0: 
	//       ||||||\____1: 
	//       |||||\_____2: 
	//       ||||\______3: 
	//       |||\_______4: 
	//       ||\________5: 
	//       |\_________6: 
	//       \__________7: 
	//PORTB reserved for programmer (use programmer pins if you know what you are doing)
	DDRB = _BV (PB6);	//PB6 is LED, hold low to turn it on
	DDRC = 0b11111111;  //configure ports for output
	//       ||||||||
	//       |||||||\___0: 
	//       ||||||\____1: 
	//       |||||\_____2: 
	//       ||||\______3: 
	//       |||\_______4: 
	//       ||\________5: 
	//       |\_________6: 
	//       \__________7: 
	DDRD = 0b11111011;  //configure ports for output
	//       ||||||||
	//       |||||||\___0: 
	//       ||||||\____1: 
	//       |||||\_____2: serial RXD1, input  -> 0
	//       ||||\______3: serial TXD1, output -> 1
	//       |||\_______4: 
	//       ||\________5: 
	//       |\_________6: 
	//       \__________7: timer0
	DDRE = 0b11111110;  //configure ports for output
	//       ||||||||
	//       |||||||\___0: serial RXD0, input  -> 0
	//       ||||||\____1: serial TXD0, output -> 1
	//       |||||\_____2: 
	//       ||||\______3: 
	//       |||\_______4: 
	//       ||\________5: 
	//       |\_________6: 
	//       \__________7: 
	//cbi(PORTG, PG5);  // disable pull-up resistor for v1e and earlier
	sbi(PORTG, PG5);  // enable pull-up resistor for v1f and later
	cbi(DDRG, PG5);	//PG5 is for the button, make a digital input
	//note that button actions are inverted between both versions!!!
	DDRH = 0b11111110;  //configure ports for output
	//       ||||||||
	//       |||||||\___0: serial RXD2, input  -> 0
	//       ||||||\____1: serial TXD2, output -> 1
	//       |||||\_____2: 
	//       ||||\______3: 
	//       |||\_______4: 
	//       ||\________5: 
	//       |\_________6: 
	//       \__________7: 
	DDRJ = 0b11111110;  //configure ports for output
	//       ||||||||
	//       |||||||\___0: serial RXD3, input  -> 0
	//       ||||||\____1: serial TXD3, output -> 1
	//       |||||\_____2: 
	//       ||||\______3: 
	//       |||\_______4: 
	//       ||\________5: 
	//       |\_________6: 
	//       \__________7: 
	//PORTL has no headers
	};
//***************************************


//************DELAY FUNCTIONS************
//wait for X amount of cycles (234 cycles is about 1.003 milliseconds)//incorrect
//to calculate: 234/1.003*(time in milliseconds) = number of cycles
void delay_cycles(unsigned long int cycles)
	{
	cycles=cycles;//doubled frequency but too lazy to change times
	while(cycles > 0)
		cycles--;
	}
//***************************************


//***************STATUS LED**************
//tank test
void LED_off(void)
	{PORT_ON(PORTB,6);}
void LED_on(void)
	{PORT_OFF(PORTB,6);}
//***************************************


//**************RESET TIMER**************
void reset_timer_0(void)
	{
	//restart timer count
	TCNT0=0x00;//clear timer
	timer0ClearOverflowCount();//clear timer0's overflow counter.
	}
void reset_timer_1(void)
	{
	//restart timer count
	TCNT1=0x00;//clear timer
	timer1ClearOverflowCount();//clear timer0's overflow counter.
	}
void reset_timer_2(void)
	{
	//restart timer count
	TCNT2=0x00;//clear timer
	timer2ClearOverflowCount();//clear timer0's overflow counter.
	}
void reset_timer_3(void)
	{
	//restart timer count
	TCNT3=0x00;//clear timer
	timer3ClearOverflowCount();//clear timer0's overflow counter.
	}
void reset_timer_4(void)
	{
	//restart timer count
	TCNT4=0x00;//clear timer
	timer4ClearOverflowCount();//clear timer0's overflow counter.
	}
void reset_timer_5(void)
	{
	//restart timer count
	TCNT5=0x00;//clear timer
	timer5ClearOverflowCount();//clear timer0's overflow counter.
	}
//***************************************


//*****************BUTTON****************
int button_pressed(void)
	{
	return (bit_is_clear(PING, 5));
	//return ((PING) & (1<<PG5));//old version, went high when button pushed
	}
//***************************************


//trig tables not yet tested . . .
//***********TRIG LOOKUP TABLES**********
//returns a trig sin or cos calculation value multiplied by 100 (to avoid floating point math)
//returns a trig tan calculation value multiplied by 10 (to avoid floating point math)
//only allows for angles between 0 and 360 degrees

//multiplied by 100 so no floating point math
signed int angtable[73]={100,100,98,97,94,91,87,82,77,71,64,57,50,42,34,26,17,9,0,-9,-17,-26,-34,-42,-50,-57,-64,-71,-77,-82,-87,-91,-94,-97,-98,-100,
						 -100,-100,-98,-97,-94,-91,-87,-82,-77,-71,-64,-57,-50,-42,-34,-26,-17,-9,0,9,17,26,34,42,50,57,64,71,77,82,87,91,94,97,98,100,100};

signed int cos_SoR(long signed int degrees)//returns cos*100
	{
	if (degrees >= 0)//positive angles
		return angtable[degrees/5];
	else
		return -angtable[72-(-degrees)/5];
	}

signed int sin_SoR(long signed int degrees)//returns sin*100
	{
	degrees=degrees - 90;//phase shift 90 degrees

	if (degrees >= 0)//positive angles
		return angtable[degrees/5];
	else
		return -angtable[72-(-degrees)/5];
	}

signed int tan_SoR(long signed int degrees)//returns tan * 10
	{
	//tan(x) = sin(x)/cos(x)
	if (degrees == 90 || degrees == -90 || degrees == 270 || degrees == -270)//blows up
		return 0;//what else should I return?!?!?
	return sin_SoR(degrees)/cos_SoR(degrees)*10;
	}

//***************************************

/*
notes to self:
int *p_variable;//initiate pointer

p_variable = &variable;//store address of variable in p_variable

printf("variable address = %d, variable = %d", p_variable, *p_variable);

----
char *p_variable stores variable of char type, int *p_variable stores variable of int type
*/
