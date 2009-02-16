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
*	This timerx8.c file has been heavily modified by SoR, but original
*	documentation has been left alone
*
****************************************************************************/
//*****************************************************************************
//
// File Name	: 'timerx8.c'
// Title		: Timer function library for ATmegaXX8 Processors
// Author		: Pascal Stang - Copyright (C) 2000-2005
// Created		: 11/22/2000
// Revised		: 06/15/2005
// Version		: 1.0
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "global.h"
#include "timer640.h"

// Program ROM constants
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
unsigned short __attribute__ ((progmem)) TimerPrescaleFactor[] = {0,1,8,64,256,1024};
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024
unsigned short __attribute__ ((progmem)) TimerRTCPrescaleFactor[] = {0,1,8,32,64,128,256,1024};

// Global variables
// time registers
volatile unsigned long TimerPauseReg;
volatile unsigned long Timer0Reg0;
volatile unsigned long Timer1Reg0;
volatile unsigned long Timer2Reg0;
volatile unsigned long Timer3Reg0;
volatile unsigned long Timer4Reg0;
volatile unsigned long Timer5Reg0;

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc[TIMER_NUM_INTERRUPTS];

// delay for a minimum of <us> microseconds 
// the time resolution is dependent on the time the loop takes 
// e.g. with 4Mhz and 5 cycles per loop, the resolution is 1.25 us 
void delay_us(unsigned short time_us) 
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (time_us+3)/5*CYCLES_PER_US; // +3 for rounding up (dirty) 

	// one loop takes 5 cpu cycles 
	for (i=0; i < delay_loops; i++) {};
}
void timerInit(void)
{
	u08 intNum;
	// detach all user functions from interrupts
	for(intNum=0; intNum<TIMER_NUM_INTERRUPTS; intNum++)
		timerDetach(intNum);

	// initialize all timers
	timer0Init();
	timer1Init();
	#ifdef TCNT2	// support timer2 only if it exists
	timer2Init();
	#endif
	#ifdef TCNT3	// support timer3 only if it exists
	timer3Init();
	#endif
	#ifdef TCNT4	// support timer4 only if it exists
	timer4Init();
	#endif
	#ifdef TCNT5	// support timer5 only if it exists
	timer5Init();
	#endif
	// enable interrupts
	sei();
}

void timer0Init()
{
	// initialize timer 0
	timer0SetPrescaler( TIMER0PRESCALE );	// set prescaler
	TCNT0 = 0;								// reset TCNT0
	sbi(TIMSK0, TOIE0);						// enable TCNT0 overflow interrupt

	timer0ClearOverflowCount();				// initialize time registers
}

void timer1Init(void)
{
	// initialize timer 1
	timer1SetPrescaler( TIMER1PRESCALE );	// set prescaler
	TCNT1 = 0;								// reset TCNT1
	sbi(TIMSK1, TOIE1);						// enable TCNT1 overflow
}

#ifdef TCNT2	// support timer2 only if it exists
void timer2Init(void)
{
	// initialize timer 2
	timer2SetPrescaler( TIMER2PRESCALE );	// set prescaler
	TCNT2 = 0;								// reset TCNT2
	sbi(TIMSK2, TOIE2);						// enable TCNT2 overflow

	timer2ClearOverflowCount();				// initialize time registers
}
#endif

#ifdef TCNT3	// support timer3 only if it exists
void timer3Init(void)
{
	// initialize timer 3
	timer3SetPrescaler( TIMER3PRESCALE );	// set prescaler
	TCNT3 = 0;								// reset TCNT3
	sbi(TIMSK3, TOIE3);						// enable TCNT3 overflow

	timer3ClearOverflowCount();				// initialize time registers
}
#endif

#ifdef TCNT4	// support timer4 only if it exists
void timer4Init(void)
{
	// initialize timer 4
	timer4SetPrescaler( TIMER4PRESCALE );	// set prescaler
	TCNT4 = 0;								// reset TCNT4
	sbi(TIMSK4, TOIE4);						// enable TCNT4 overflow

	timer4ClearOverflowCount();				// initialize time registers
}
#endif

#ifdef TCNT5	// support timer5 only if it exists
void timer5Init(void)
{
	// initialize timer 5
	timer5SetPrescaler( TIMER5PRESCALE );	// set prescaler
	TCNT5 = 0;								// reset TCNT5
	sbi(TIMSK5, TOIE5);						// enable TCNT5 overflow

	timer5ClearOverflowCount();				// initialize time registers
}
#endif

void timer0SetPrescaler(u08 prescale)
{
	// set prescaler on timer 0
	TCCR0B = ((TCCR0B & ~TIMER_PRESCALE_MASK) | prescale);
}

void timer1SetPrescaler(u08 prescale)
{
	// set prescaler on timer 1
	TCCR1B = ((TCCR1B & ~TIMER_PRESCALE_MASK) | prescale);
}

#ifdef TCNT2	// support timer2 only if it exists
void timer2SetPrescaler(u08 prescale)
{
	// set prescaler on timer 2
	TCCR2B = ((TCCR2B & ~TIMER_PRESCALE_MASK) | prescale);
}
#endif

#ifdef TCNT3	// support timer3 only if it exists
void timer3SetPrescaler(u08 prescale)
{
	// set prescaler on timer 3
	TCCR3B = ((TCCR3B & ~TIMER_PRESCALE_MASK) | prescale);
}
#endif

#ifdef TCNT4	// support timer4 only if it exists
void timer4SetPrescaler(u08 prescale)
{
	// set prescaler on timer 4
	TCCR4B = ((TCCR4B & ~TIMER_PRESCALE_MASK) | prescale);
}
#endif

#ifdef TCNT5	// support timer5 only if it exists
void timer5SetPrescaler(u08 prescale)
{
	// set prescaler on timer 5
	TCCR5B = ((TCCR5B & ~TIMER_PRESCALE_MASK) | prescale);
}
#endif

u16 timer0GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(TCCR0B & TIMER_PRESCALE_MASK)));
}

u16 timer1GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(TCCR1B & TIMER_PRESCALE_MASK)));
}

#ifdef TCNT2	// support timer2 only if it exists
u16 timer2GetPrescaler(void)
{
	//TODO: can we assume for all 3-timer AVR processors,
	// that timer2 is the RTC timer?

	// get the current prescaler setting
	return (pgm_read_word(TimerRTCPrescaleFactor+(TCCR2B & TIMER_PRESCALE_MASK)));
}
#endif

#ifdef TCNT3	// support timer3 only if it exists
u16 timer3GetPrescaler(void)
{
	//TODO: can we assume for all 3-timer AVR processors,
	// that timer3 is the RTC timer?

	// get the current prescaler setting
	return (pgm_read_word(TimerRTCPrescaleFactor+(TCCR3B & TIMER_PRESCALE_MASK)));
}
#endif

#ifdef TCNT4	// support timer4 only if it exists
u16 timer4GetPrescaler(void)
{
	//TODO: can we assume for all 3-timer AVR processors,
	// that timer4 is the RTC timer?

	// get the current prescaler setting
	return (pgm_read_word(TimerRTCPrescaleFactor+(TCCR4B & TIMER_PRESCALE_MASK)));
}
#endif

#ifdef TCNT5	// support timer5 only if it exists
u16 timer5GetPrescaler(void)
{
	//TODO: can we assume for all 3-timer AVR processors,
	// that timer5 is the RTC timer?

	// get the current prescaler setting
	return (pgm_read_word(TimerRTCPrescaleFactor+(TCCR5B & TIMER_PRESCALE_MASK)));
}
#endif

void timerAttach(u08 interruptNum, void (*userFunc)(void) )
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		TimerIntFunc[interruptNum] = userFunc;
	}
}

void timerDetach(u08 interruptNum)
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run nothing
		TimerIntFunc[interruptNum] = 0;
	}
}
void timerPause(unsigned short pause_ms)
{
	// pauses for exactly <pause_ms> number of milliseconds
	u08 timerThres;
	u32 ticRateHz;
	u32 pause;

	// capture current pause timer value
	timerThres = TCNT0;
	// reset pause timer overflow count
	TimerPauseReg = 0;
	// calculate delay for [pause_ms] milliseconds
	// prescaler division = 1<<(pgm_read_byte(TimerPrescaleFactor+inb(TCCR0)))
	ticRateHz = F_CPU/timer0GetPrescaler();
	// precision management
	// prevent overflow and precision underflow
	//	-could add more conditions to improve accuracy
	if( ((ticRateHz < 429497) && (pause_ms <= 10000)) )
		pause = (pause_ms*ticRateHz)/1000;
	else
		pause = pause_ms*(ticRateHz/1000);

	// loop until time expires
	while( ((TimerPauseReg<<8) | (TCNT0)) < (pause+timerThres) )
	{
		if( TimerPauseReg < (pause>>8));
		{
			// save power by idling the processor
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
		}
	}
}

void timer0ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer0Reg0 = 0;	// initialize time registers
}

long timer0GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer0ClearOverflowCount() command was called)
	return Timer0Reg0;
}

void timer1ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer1Reg0 = 0;	// initialize time registers
}

long timer1GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer1ClearOverflowCount() command was called)
	return Timer1Reg0;
}

#ifdef TCNT2	// support timer2 only if it exists
void timer2ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer2Reg0 = 0;	// initialize time registers
}

long timer2GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer2ClearOverflowCount() command was called)
	return Timer2Reg0;
}
#endif

#ifdef TCNT3	// support timer3 only if it exists
void timer3ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer3Reg0 = 0;	// initialize time registers
}

long timer3GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer3ClearOverflowCount() command was called)
	return Timer3Reg0;
}
#endif

#ifdef TCNT4	// support timer4 only if it exists
void timer4ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer4Reg0 = 0;	// initialize time registers
}

long timer4GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer4ClearOverflowCount() command was called)
	return Timer4Reg0;
}
#endif

#ifdef TCNT5	// support timer5 only if it exists
void timer5ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer5Reg0 = 0;	// initialize time registers
}

long timer5GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer5ClearOverflowCount() command was called)
	return Timer5Reg0;
}
#endif

/*
ATmega640: Four 8-bit PWM Channels, Six/Twelve PWM
Channels with Programmable Resolution from 2 to 16 Bits

PWM pins on Axon:
OC0A  B7 (not connected)
OC0B  G5 (attached to button)
OC1A  B5 (not connected)
OC1B  B6 (attached to green LED) timer2
OC1C  B7 (not connected)
OC2A  B4 (not connected)
OC2B  H6 timer2
OC3A  E3 timer3
OC3B  E4 timer3
OC3C  E5 timer3
OC4A  H3 timer0
OC4B  H4 timer0
OC4C  H5 timer0
OC5A  L3 (not connected)
OC5B  L4 (not connected)
OC5C  L5 (not connected)
*/

void timer0PWMInit(u08 bitRes)
{
	// configures timer0 for use with PWM output
	// on OC0A and OC0B pins

	// enable timer0 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR0A,PWM11);
		cbi(TCCR0A,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR0A,PWM11);
		sbi(TCCR0A,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR0A,PWM11);
		sbi(TCCR0A,PWM10);
	}

	// clear output compare values
	OCR4A = 0;
	OCR4B = 0;
	OCR4C = 0;
}

void timer1PWMInit(u08 bitRes)
{
	// configures timer1 for use with PWM output
	// on OC1A and OC1B pins

	// enable timer1 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR1A,PWM11);
		cbi(TCCR1A,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR1A,PWM11);
		sbi(TCCR1A,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR1A,PWM11);
		sbi(TCCR1A,PWM10);
	}

	// clear output compare values
	OCR1A = 0;
	OCR1B = 0;
	OCR1C = 0;
}

//works on pin H6
void timer2PWMInit(u08 bitRes)
{
	// configures timer2 for use with PWM output
	// on OC2A and OC2B pins

	// enable timer2 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR2A,PWM11);
		cbi(TCCR2A,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR2A,PWM11);
		sbi(TCCR2A,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR2A,PWM11);
		sbi(TCCR2A,PWM10);
	}

	// clear output compare values
	OCR1B = 0;
	OCR2B = 0;
}

#ifdef WGM10
// include support for arbitrary top-count PWM
// on new AVR processors that support it
void timer1PWMInitICR(u16 topcount)
{
	// set PWM mode with ICR top-count
	cbi(TCCR1A,WGM10);
	sbi(TCCR1A,WGM11);
	sbi(TCCR1B,WGM12);
	sbi(TCCR1B,WGM13);
	
	// set top count value
	ICR1 = topcount;
	
	// clear output compare values
	OCR1A = 0;
	OCR1B = 0;
	OCR1C = 0;

}
void timer3PWMInitICR(u16 topcount)
{
	// set PWM mode with ICR top-count
	cbi(TCCR3A,WGM10);
	sbi(TCCR3A,WGM11);
	sbi(TCCR3B,WGM12);
	sbi(TCCR3B,WGM13);
	
	// set top count value
	ICR3 = topcount;
	
	// clear output compare values
	OCR3A = 0;
	OCR3B = 0;
	OCR3C = 0;

}
void timer4PWMInitICR(u16 topcount)
{
	// set PWM mode with ICR top-count
	cbi(TCCR4A,WGM10);
	sbi(TCCR4A,WGM11);
	sbi(TCCR4B,WGM12);
	sbi(TCCR4B,WGM13);
	
	// set top count value
	ICR4 = topcount;
	
	// clear output compare values
	OCR4A = 0;
	OCR4B = 0;
	OCR4C = 0;

}
void timer5PWMInitICR(u16 topcount)
{
	// set PWM mode with ICR top-count
	cbi(TCCR5A,WGM10);
	sbi(TCCR5A,WGM11);
	sbi(TCCR5B,WGM12);
	sbi(TCCR5B,WGM13);
	
	// set top count value
	ICR5 = topcount;
	
	// clear output compare values
	OCR5A = 0;
	OCR5B = 0;
	OCR5C = 0;

}
#endif

void timer0PWMOff(void)
{
	// turn off timer0 PWM mode
	cbi(TCCR0A,PWM11);
	cbi(TCCR0A,PWM10);
	// set PWM0A/B (OutputCompare action) to none
	timer0PWMAOff();
	timer0PWMBOff();
	timer0PWMCOff();
}
void timer1PWMOff(void)
{
	// turn off timer1 PWM mode
	cbi(TCCR1A,PWM11);
	cbi(TCCR1A,PWM10);
	// set PWM1A/B (OutputCompare action) to none
	timer1PWMAOff();
	timer1PWMBOff();
	timer1PWMCOff();
}
void timer2PWMOff(void)
{
	// turn off timer2 PWM mode
	cbi(TCCR2A,PWM11);
	cbi(TCCR2A,PWM10);
	// set PWM2A/B (OutputCompare action) to none
	//timer2PWMAOff();
	timer2PWMBOff();
	//timer2PWMCOff();
}

void timer0PWMAOn(void)
{
	// turn on channel A (OC0A) PWM output
	// set OC1A as non-inverted PWM
	sbi(TCCR0A,COM4A1);
	cbi(TCCR0A,COM4A0);
}

void timer1PWMAOn(void)
{
	// turn on channel A (OC1A) PWM output
	// set OC1A as non-inverted PWM
	sbi(TCCR1A,COM1A1);
	cbi(TCCR1A,COM1A0);
}

void timer2PWMAOn(void)
{
	// turn on channe2 A (OC2A) PWM output
	// set OC2A as non-inverted PWM
	sbi(TCCR2A,COM1A1);
	cbi(TCCR2A,COM1A0);
}

void timer0PWMBOn(void)
{
	// turn on channel B (OC0B) PWM output
	// set OC0B as non-inverted PWM
	sbi(TCCR0A,COM4B1);
	cbi(TCCR0A,COM4B0);
}

void timer1PWMBOn(void)
{
	// turn on channel B (OC1B) PWM output
	// set OC1B as non-inverted PWM
	sbi(TCCR1A,COM1B1);
	cbi(TCCR1A,COM1B0);
}

void timer2PWMBOn(void)
{
	// turn on channel B (OC2B) PWM output
	// set OC2B as non-inverted PWM
	sbi(TCCR2A,COM1B1);
	cbi(TCCR2A,COM1B0);
}

void timer0PWMCOn(void)
{
	// turn on channel B (OC0B) PWM output
	// set OC0B as non-inverted PWM
	sbi(TCCR0A,COM4C1);
	cbi(TCCR0A,COM4C0);
}

void timer1PWMCOn(void)
{
	// turn on channel B (OC1B) PWM output
	// set OC1B as non-inverted PWM
	sbi(TCCR1A,COM1C1);
	cbi(TCCR1A,COM1C0);
}

void timer2PWMCOn(void)
{
	// turn on channel B (OC2B) PWM output
	// set OC2B as non-inverted PWM
	sbi(TCCR2A,COM1C1);
	cbi(TCCR2A,COM1C0);
}

void timer0PWMAOff(void)
{
	// turn off channel A (OC0A) PWM output
	// set OC0A (OutputCompare action) to none
	cbi(TCCR0A,COM4A1);
	cbi(TCCR0A,COM4A0);
}

void timer1PWMAOff(void)
{
	// turn off channel A (OC1A) PWM output
	// set OC1A (OutputCompare action) to none
	cbi(TCCR1A,COM1A1);
	cbi(TCCR1A,COM1A0);
}

void timer2PWMAOff(void)
{
	// turn off channel A (OC2A) PWM output
	// set OC2A (OutputCompare action) to none
	cbi(TCCR2A,COM1A1);
	cbi(TCCR2A,COM1A0);
}

void timer0PWMBOff(void)
{
	// turn off channel B (OC0B) PWM output
	// set OC0B (OutputCompare action) to none
	cbi(TCCR0A,COM4B1);
	cbi(TCCR0A,COM4B0);
}

void timer1PWMBOff(void)
{
	// turn off channel B (OC1B) PWM output
	// set OC1B (OutputCompare action) to none
	cbi(TCCR1A,COM1B1);
	cbi(TCCR1A,COM1B0);
}

void timer2PWMBOff(void)
{
	// turn off channel B (OC2B) PWM output
	// set OC2B (OutputCompare action) to none
	cbi(TCCR2A,COM1B1);
	cbi(TCCR2A,COM1B0);
}

void timer0PWMCOff(void)
{
	// turn off channel B (OC0B) PWM output
	// set OC0B (OutputCompare action) to none
	cbi(TCCR0A,COM4C1);
	cbi(TCCR0A,COM4C0);
}

void timer1PWMCOff(void)
{
	// turn off channel B (OC1B) PWM output
	// set OC1B (OutputCompare action) to none
	cbi(TCCR1A,COM1C1);
	cbi(TCCR1A,COM1C0);
}

void timer2PWMCOff(void)
{
	// turn off channel B (OC2B) PWM output
	// set OC2B (OutputCompare action) to none
	cbi(TCCR2A,COM1C1);
	cbi(TCCR2A,COM1C0);
}

void timer0PWMASet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel A
	// this PWM output is generated on OC0A pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	OCR4A = pwmDuty;
}

void timer1PWMASet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel A
	// this PWM output is generated on OC1A pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	OCR1A = pwmDuty;
}

void timer2PWMASet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel A
	// this PWM output is generated on OC2A pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	OCR2A = pwmDuty;
}

void timer0PWMBSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC0B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	OCR4B = pwmDuty;
}

void timer1PWMBSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC1B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	OCR1B = pwmDuty;
}

void timer2PWMBSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC2B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	OCR2B = pwmDuty;
}

void timer0PWMCSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC0B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	OCR4C = pwmDuty;
}

void timer1PWMCSet(u16 pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC1B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	OCR1C = pwmDuty;
}


//! Interrupt handler for tcnt0 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW0)
{
	Timer0Reg0++;			// increment low-order counter

	// increment pause counter
	TimerPauseReg++;

	// if a user function is defined, execute it too
	if(TimerIntFunc[TIMER0OVERFLOW_INT])
		TimerIntFunc[TIMER0OVERFLOW_INT]();
}

//! Interrupt handler for tcnt1 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW1)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OVERFLOW_INT])
		TimerIntFunc[TIMER1OVERFLOW_INT]();
}

#ifdef TCNT2	// support timer2 only if it exists
//! Interrupt handler for tcnt2 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW2)
{
	Timer2Reg0++;			// increment low-order counter

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OVERFLOW_INT])
		TimerIntFunc[TIMER2OVERFLOW_INT]();
}
#endif

#ifdef TCNT3	// support timer3 only if it exists
//! Interrupt handler for tcnt3 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW3)
{
	Timer3Reg0++;			// increment low-order counter

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3OVERFLOW_INT])
		TimerIntFunc[TIMER3OVERFLOW_INT]();
}
#endif

#ifdef TCNT4	// support timer4 only if it exists
//! Interrupt handler for tcnt4 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW4)
{
	Timer4Reg0++;			// increment low-order counter

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER4OVERFLOW_INT])
		TimerIntFunc[TIMER4OVERFLOW_INT]();
}
#endif

#ifdef TCNT5	// support timer5 only if it exists
//! Interrupt handler for tcnt5 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OVERFLOW5)
{
	Timer5Reg0++;			// increment low-order counter

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER5OVERFLOW_INT])
		TimerIntFunc[TIMER5OVERFLOW_INT]();
}
#endif

#ifdef OCR0
// include support for Output Compare 0 for new AVR processors that support it
//! Interrupt handler for OutputCompare0 match (OC0) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE0)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPARE_INT])
		TimerIntFunc[TIMER0OUTCOMPARE_INT]();
}
#endif

//! Interrupt handler for CutputCompare0A match (OC0A) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE0A)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPAREA_INT])
		TimerIntFunc[TIMER0OUTCOMPAREA_INT]();
}

//! Interrupt handler for CutputCompare1A match (OC1A) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1A)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREA_INT])
		TimerIntFunc[TIMER1OUTCOMPAREA_INT]();
}

//! Interrupt handler for CutputCompare2A match (OC2A) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE2A)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OUTCOMPAREA_INT])
		TimerIntFunc[TIMER2OUTCOMPAREA_INT]();
}

//! Interrupt handler for OutputCompare0B match (OC0B) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE0B)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPAREB_INT])
		TimerIntFunc[TIMER0OUTCOMPAREB_INT]();
}

//! Interrupt handler for OutputCompare1B match (OC1B) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE1B)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREB_INT])
		TimerIntFunc[TIMER1OUTCOMPAREB_INT]();
}

//! Interrupt handler for OutputCompare2B match (OC2B) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE2B)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OUTCOMPAREB_INT])
		TimerIntFunc[TIMER2OUTCOMPAREB_INT]();
}

//! Interrupt handler for InputCapture0 (IC0) interrupt
TIMER_INTERRUPT_HANDLER(SIG_INPUT_CAPTURE0)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0INPUTCAPTURE_INT])
		TimerIntFunc[TIMER0INPUTCAPTURE_INT]();
}

//! Interrupt handler for InputCapture1 (IC1) interrupt
TIMER_INTERRUPT_HANDLER(SIG_INPUT_CAPTURE1)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1INPUTCAPTURE_INT])
		TimerIntFunc[TIMER1INPUTCAPTURE_INT]();
}

//! Interrupt handler for InputCapture2 (IC2) interrupt
TIMER_INTERRUPT_HANDLER(SIG_INPUT_CAPTURE2)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2INPUTCAPTURE_INT])
		TimerIntFunc[TIMER2INPUTCAPTURE_INT]();
}
