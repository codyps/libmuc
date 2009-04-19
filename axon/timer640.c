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

/*
Two 8-bit Timer/Counters with Separate Prescaler and Compare Mode
Four 16-bit Timer/Counter with Separate Prescaler, Compare- and Capture Mode
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "global.h"
#include "timer640.h"
#include "rprintf.h"

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
//rprintf("sleep2");
			// save power by idling the processor
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
//rprintf("sleep3");
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
OC1B  B6 (attached to green LED)
OC1C  B7 (not connected)
OC2A  B4 (not connected)
OC2B  H6
OC3A  E3
OC3B  E4
OC3C  E5
OC4A  H3
OC4B  H4
OC4C  H5
OC5A  L3 (not connected)
OC5B  L4 (not connected)
OC5C  L5 (not connected)

explainations/examples:
http://www.societyofrobots.com/robotforum/index.php?topic=1827.0
http://www.societyofrobots.com/robotforum/index.php?topic=5590.0

If you use this, then you cannot use the associated timers for other things.
H6 	 uses timer2
E3-5 uses timer3
H3-5 uses timer4
Unfortunately, the 3 pins that use timer5 are not connected on the Axon board, so they are
not available for PWM. The upside is that timer5 is available for other use without interference.

OCR is the PWM on from 0, while ICR is the total PWM length

|----|___________________
  OCR
  		  ICR

to calculate PWM: (desired ICR time, seconds)*(clock frequency, 1/seconds)/prescaler = TOP
TOP=TOP/2 if using phase and frequency correct mode

adjust the duty cycle of the output pin by setting OCR1A to a value between 0 and ICR1

PWM tutorial: http://www.societyofrobots.com/member_tutorials/node/228
*/

//tested and working: H3, H6

//OC1B  pin B6 (attached to green LED)
void PWM_Init_timer1_LED(u08 bitRes)
{
	// enable timer2 as 8,9,10bit PWM
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
	OCR1B = 0;
}
//pin H6, timer2
void PWM_Init_timer2_H6(u08 bitRes)
{
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
	OCR2B = 0;
}
//pin E3
void PWM_Init_timer3_E3(u08 bitRes)
{
	// enable timer3 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR3A,PWM11);
		cbi(TCCR3A,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR3A,PWM11);
		sbi(TCCR3A,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR3A,PWM11);
		sbi(TCCR3A,PWM10);
	}
	// clear output compare values
	OCR3A = 0;
	//timer3PWMInitICR(20000);// 20mS PWM cycle time for RC servos
}
//pin E4
void PWM_Init_timer3_E4(u08 bitRes)
{
	// enable timer3 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR3B,PWM11);
		cbi(TCCR3B,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR3B,PWM11);
		sbi(TCCR3B,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR3B,PWM11);
		sbi(TCCR3B,PWM10);
	}
	// clear output compare values
	OCR3B = 0;
	//timer3PWMInitICR(20000);// 20mS PWM cycle time for RC servos
}
//pin E5
void PWM_Init_timer3_E5(u08 bitRes)
{
	// enable timer3 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR3C,PWM11);
		cbi(TCCR3C,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR3C,PWM11);
		sbi(TCCR3C,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR3C,PWM11);
		sbi(TCCR3C,PWM10);
	}
	// clear output compare values
	OCR3C = 0;
	//timer3PWMInitICR(20000);// 20mS PWM cycle time for RC servos
}
//pin H3, timer4
void PWM_Init_timer4_H3(u08 bitRes)
{
	// enable timer4 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR4A,PWM11);
		cbi(TCCR4A,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR4A,PWM11);
		sbi(TCCR4A,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR4A,PWM11);
		sbi(TCCR4A,PWM10);
	}
	// clear output compare values
	OCR4A = 0;
	//timer4PWMInitICR(20000);// 20mS PWM cycle time for RC servos
}
//pin H4, timer4
void PWM_Init_timer4_H4(u08 bitRes)
{
	// enable timer4 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR4B,PWM11);
		cbi(TCCR4B,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR4B,PWM11);
		sbi(TCCR4B,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR4B,PWM11);
		sbi(TCCR4B,PWM10);
	}
	// clear output compare values
	OCR4B = 0;
	//timer4PWMInitICR(20000);// 20mS PWM cycle time for RC servos
}
//pin H5, timer4
void PWM_Init_timer4_H5(u08 bitRes)
{
	// enable timer4 as 8,9,10bit PWM
	if(bitRes == 9)
	{	// 9bit mode
		sbi(TCCR4A,PWM11);
		cbi(TCCR4A,PWM10);
	}
	else if( bitRes == 10 )
	{	// 10bit mode
		sbi(TCCR4A,PWM11);
		sbi(TCCR4A,PWM10);
	}
	else
	{	// default 8bit mode
		cbi(TCCR4A,PWM11);
		sbi(TCCR4A,PWM10);
	}
	// clear output compare values
	OCR4C = 0;
	//timer4PWMInitICR(20000);// 20mS PWM cycle time for RC servos
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
#endif

//on commands
void PWM_timer1_On_LED(void)
{
	sbi(TCCR1A,COM1B1);
	cbi(TCCR1A,COM1B0);
}
void PWM_timer2_On_H6(void)
{
	sbi(TCCR2A,COM2B1);
	cbi(TCCR2A,COM2B0);
}
void PWM_timer3_On_E3(void)
{
	sbi(TCCR3A,COM3A1);
	cbi(TCCR3A,COM3A0);
}
void PWM_timer3_On_E4(void)
{
	sbi(TCCR3A,COM3B1);
	cbi(TCCR3A,COM3B0);
}
void PWM_timer3_On_E5(void)
{
	sbi(TCCR3A,COM3C1);
	cbi(TCCR3A,COM3C0);
}
void PWM_timer4_On_H3(void)
{
	sbi(TCCR4A,COM4A1);
	cbi(TCCR4A,COM4A0);
}
void PWM_timer4_On_H4(void)
{
	sbi(TCCR4A,COM4B1);
	cbi(TCCR4A,COM4B0);
}
void PWM_timer4_On_H5(void)
{
	sbi(TCCR4A,COM4C1);
	cbi(TCCR4A,COM4C0);
}

//off commands
void PWM_timer1_Off_LED(void)
{
	cbi(TCCR1A,COM1B1);
	cbi(TCCR1A,COM1B0);
}
void PWM_timer2_Off_H6(void)
{
	cbi(TCCR2A,COM2B1);
	cbi(TCCR2A,COM2B0);
}
void PWM_timer3_Off_E3(void)
{
	cbi(TCCR3A,COM3A1);
	cbi(TCCR3A,COM3A0);
}
void PWM_timer3_Off_E4(void)
{
	cbi(TCCR3A,COM3B1);
	cbi(TCCR3A,COM3B0);
}
void PWM_timer3_Off_E5(void)
{
	cbi(TCCR3A,COM3C1);
	cbi(TCCR3A,COM3C0);
}
void PWM_timer4_Off_H3(void)
{
	cbi(TCCR4A,COM4A1);
	cbi(TCCR4A,COM4A0);
}
void PWM_timer4_Off_H4(void)
{
	cbi(TCCR4A,COM4B1);
	cbi(TCCR4A,COM4B0);
}
void PWM_timer4_Off_H5(void)
{
	cbi(TCCR4A,COM4C1);
	cbi(TCCR4A,COM4C0);
}


void PWM_timer1_Off_All(void)
{
	cbi(TCCR1A,PWM11);
	cbi(TCCR1A,PWM10);
	//timer2PWMAOff();
	PWM_timer1_Off_LED();
	//timer2PWMCOff();
}
void PWM_timer2_Off_All(void)
{
	cbi(TCCR2A,PWM11);
	cbi(TCCR2A,PWM10);
	//timer2PWMAOff();
	PWM_timer2_Off_H6();
	//timer2PWMCOff();
}
void PWM_timer3_Off_All(void)
{
	cbi(TCCR3A,PWM11);
	cbi(TCCR3A,PWM10);
	//timer2PWMAOff();
	PWM_timer3_Off_E3();
	PWM_timer3_Off_E4();
	PWM_timer3_Off_E5();
	//timer2PWMCOff();
}
void PWM_timer4_Off_All(void)
{
	cbi(TCCR4A,PWM11);
	cbi(TCCR4A,PWM10);
	//timer2PWMAOff();
	PWM_timer4_Off_H3();
	PWM_timer4_Off_H4();
	PWM_timer4_Off_H5();
	//timer2PWMCOff();
}


// set PWM (output compare) duty for channel B
// this PWM output is generated on OC2B pin
// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
//			pwmDuty should be in the range 0-511 for 9bit PWM
//			pwmDuty should be in the range 0-1023 for 10bit PWM
void PWM_timer1_Set_LED(u16 pwmDuty)
	{OCR1B = pwmDuty;}
void PWM_timer2_Set_H6(u16 pwmDuty)
	{OCR2B = pwmDuty;}
void PWM_timer3_Set_E3(u16 pwmDuty)
	{OCR3A = pwmDuty;}
void PWM_timer3_Set_E4(u16 pwmDuty)
	{OCR3B = pwmDuty;}
void PWM_timer3_Set_E5(u16 pwmDuty)
	{OCR3C = pwmDuty;}
void PWM_timer4_Set_H3(u16 pwmDuty)
	{OCR4A = pwmDuty;}
void PWM_timer4_Set_H4(u16 pwmDuty)
	{OCR4B = pwmDuty;}
void PWM_timer4_Set_H5(u16 pwmDuty)
	{OCR4C = pwmDuty;}








//INTERRUPTS
// not sure why, but this interrupt is being called.  I'm not sure what the PD0 pin is hooked up to on the Axon.
// this is basically the cause of the reset.
EMPTY_INTERRUPT(INT0_vect);

//! Interrupt handler for tcnt0 overflow interrupt
ISR(TIMER0_OVF_vect)
{
	Timer0Reg0++;			// increment low-order counter

	// increment pause counter
	TimerPauseReg++;

	// if a user function is defined, execute it too
	if(TimerIntFunc[TIMER0OVERFLOW_INT])
		TimerIntFunc[TIMER0OVERFLOW_INT]();
}

//! Interrupt handler for tcnt1 overflow interrupt
ISR(TIMER1_OVF_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OVERFLOW_INT])
		TimerIntFunc[TIMER1OVERFLOW_INT]();
}

#ifdef TCNT2	// support timer2 only if it exists
//! Interrupt handler for tcnt2 overflow interrupt
ISR(TIMER2_OVF_vect)
{
	Timer2Reg0++;			// increment low-order counter

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OVERFLOW_INT])
		TimerIntFunc[TIMER2OVERFLOW_INT]();
}
#endif

#ifdef TCNT3	// support timer3 only if it exists
//! Interrupt handler for tcnt3 overflow interrupt
ISR(TIMER3_OVF_vect)
{
	Timer3Reg0++;			// increment low-order counter

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER3OVERFLOW_INT])
		TimerIntFunc[TIMER3OVERFLOW_INT]();
}
#endif

#ifdef TCNT4	// support timer4 only if it exists
//! Interrupt handler for tcnt4 overflow interrupt
ISR(TIMER4_OVF_vect)
{
	Timer4Reg0++;			// increment low-order counter

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER4OVERFLOW_INT])
		TimerIntFunc[TIMER4OVERFLOW_INT]();
}
#endif

#ifdef TCNT5	// support timer5 only if it exists
//! Interrupt handler for tcnt5 overflow interrupt
ISR(TIMER5_OVF_vect)
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
ISR(TIMER0_COMPA_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPARE_INT])
		TimerIntFunc[TIMER0OUTCOMPARE_INT]();
}

#endif

//! Interrupt handler for CutputCompare0A match (OC0A) interrupt
ISR(TIMER0_COMPA_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPAREA_INT])
		TimerIntFunc[TIMER0OUTCOMPAREA_INT]();
}

//! Interrupt handler for CutputCompare1A match (OC1A) interrupt
ISR(TIMER1_COMPA_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREA_INT])
		TimerIntFunc[TIMER1OUTCOMPAREA_INT]();
}

//! Interrupt handler for CutputCompare2A match (OC2A) interrupt
ISR(TIMER2_COMPA_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OUTCOMPAREA_INT])
		TimerIntFunc[TIMER2OUTCOMPAREA_INT]();
}

//! Interrupt handler for OutputCompare0B match (OC0B) interrupt
ISR(TIMER0_COMPB_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPAREB_INT])
		TimerIntFunc[TIMER0OUTCOMPAREB_INT]();
}

//! Interrupt handler for OutputCompare1B match (OC1B) interrupt
ISR(TIMER1_COMPB_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREB_INT])
		TimerIntFunc[TIMER1OUTCOMPAREB_INT]();
}

//! Interrupt handler for OutputCompare2B match (OC2B) interrupt
ISR(TIMER2_COMPB_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OUTCOMPAREB_INT])
		TimerIntFunc[TIMER2OUTCOMPAREB_INT]();
}
/*
//! Interrupt handler for InputCapture0 (IC0) interrupt
TIMER_INTERRUPT_HANDLER(SIG_INPUT_CAPTURE0)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0INPUTCAPTURE_INT])
		TimerIntFunc[TIMER0INPUTCAPTURE_INT]();
}
*/
//! Interrupt handler for InputCapture1 (IC1) interrupt
ISR(TIMER1_CAPT_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1INPUTCAPTURE_INT])
		TimerIntFunc[TIMER1INPUTCAPTURE_INT]();
}
/*
//! Interrupt handler for InputCapture2 (IC2) interrupt
TIMER_INTERRUPT_HANDLER(SIG_INPUT_CAPTURE2)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2INPUTCAPTURE_INT])
		TimerIntFunc[TIMER2INPUTCAPTURE_INT]();
}
*/

 /* need this to capture any unexpected interrupts, this causes the reset.
  * http://www.gnu.org/savannah-checkouts/non-gnu/avr-libc/user-manual/group__avr__interrupts.html
  * If an unexpected interrupt occurs (interrupt is enabled and no handler 
  * is installed, which usually indicates a bug), then the default action is
  * to reset the device by jumping to the reset vector. You can override this 
  * by supplying a function named BADISR_vect which should be defined with ISR() 
  * as such. */
ISR(BADISR_vect)
{
  rprintf("badISR called");
}
