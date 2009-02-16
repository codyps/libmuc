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
*	This timerx8.h file has been heavily modified by SoR, but original
*	documentation has been left alone
*
****************************************************************************/
//*****************************************************************************
//
// File Name	: 'timerx8.h'
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
///	\ingroup driver_avr
/// \defgroup timerx8 Timer Function Library for ATmegaXX8 (timerx8.c)
/// \code #include "timerx8.h" \endcode
/// \par Overview
///		This library provides functions for use with the timers internal
///		to the AVR processors.  Functions include initialization, set prescaler,
///		calibrated pause function (in milliseconds), attaching and detaching of
///		user functions to interrupts, overflow counters, PWM. Arbitrary
///		frequency generation has been moved to the Pulse Library.
///
/// \par About Timers
///		The Atmel AVR-series processors each contain at least one
///		hardware timer/counter.  Many of the processors contain 2 or 3
///		timers.  Generally speaking, a timer is a hardware counter inside
///		the processor which counts at a rate related to the main CPU clock
///		frequency.  Because the counter value increasing (counting up) at
///		a precise rate, we can use it as a timer to create or measure 
///		precise delays, schedule events, or generate signals of a certain
///		frequency or pulse-width.
/// \par
///		As an example, the ATmega163 processor has 3 timer/counters.
///		Timer0, Timer1, and Timer2 are 8, 16, and 8 bits wide respectively.
///		This means that they overflow, or roll over back to zero, at a
///		count value of 256 for 8bits or 65536 for 16bits.  A prescaler is
///		avaiable for each timer, and the prescaler allows you to pre-divide
///		the main CPU clock rate down to a slower speed before feeding it to
///		the counting input of a timer.  For example, if the CPU clock
///		frequency is 3.69MHz, and Timer0's prescaler is set to divide-by-8,
///		then Timer0 will "tic" at 3690000/8 = 461250Hz.  Because Timer0 is
///		an 8bit timer, it will count to 256 in just 256/461250Hz = 0.555ms.
///		In fact, when it hits 255, it will overflow and start again at
///		zero.  In this case, Timer0 will overflow 461250/256 = 1801.76
///		times per second.
/// \par
///		Timer0 can be used a number of ways simultaneously.  First, the
///		value of the timer can be read by accessing the CPU register \c TCNT0.
///		We could, for example, figure out how long it takes to execute a
///		C command by recording the value of \c TCNT0 before and after
///		execution, then subtract (after-before) = time elapsed.  Or we can
///		enable the overflow interrupt which goes off every time T0
///		overflows and count out longer delays (multiple overflows), or
///		execute a special periodic function at every overflow.
/// \par
///		The other timers (Timer1 and Timer2) offer all the abilities of
///		Timer0 and many more features.  Both T1 and T2 can operate as
///		general-purpose timers, but T1 has special hardware allowing it to
///		generate PWM signals, while T2 is specially designed to help count
///		out real time (like hours, minutes, seconds).  See the
///		Timer/Counter section of the processor datasheet for more info.
///
//*****************************************************************************
//@{

#ifndef TIMER_H
#define TIMER_H

#include "global.h"

// constants/macros/typdefs

// processor compatibility fixes
#ifdef __AVR_ATmega323__
	// redefinition for the Mega323
	#define CTC1	CTC10
#endif
#ifndef PWM10
	// mega128 PWM bits
	#define PWM10	WGM10
	#define PWM11	WGM11
#endif


// Timer/clock prescaler values and timer overflow rates
// tics = rate at which the timer counts up
// 8bitoverflow = rate at which the timer overflows 8bits (or reaches 256)
// 16bit [overflow] = rate at which the timer overflows 16bits (65536)
// 
// overflows can be used to generate periodic interrupts
//
// for 8MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 8MHz			8bitoverflow= 31250Hz		16bit= 122.070Hz
// 2 = CLOCK/8		tics= 1MHz			8bitoverflow= 3906.25Hz		16bit=  15.259Hz
// 3 = CLOCK/64		tics= 125kHz		8bitoverflow=  488.28Hz		16bit=   1.907Hz
// 4 = CLOCK/256	tics= 31250Hz		8bitoverflow=  122.07Hz		16bit=	0.477Hz
// 5 = CLOCK/1024	tics= 7812.5Hz		8bitoverflow=   30.52Hz		16bit=   0.119Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 4MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 4MHz			8bitoverflow= 15625Hz		16bit=  61.035Hz
// 2 = CLOCK/8		tics= 500kHz		8bitoverflow= 1953.125Hz	16bit=   7.629Hz
// 3 = CLOCK/64		tics= 62500Hz		8bitoverflow=  244.141Hz	16bit=   0.954Hz
// 4 = CLOCK/256	tics= 15625Hz		8bitoverflow=   61.035Hz	16bit=   0.238Hz
// 5 = CLOCK/1024	tics= 3906.25Hz		8bitoverflow=   15.259Hz	16bit=   0.060Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 3.69MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 3.69MHz		8bitoverflow= 14414Hz		16bit=  56.304Hz
// 2 = CLOCK/8		tics= 461250Hz		8bitoverflow= 1801.758Hz	16bit=   7.038Hz
// 3 = CLOCK/64		tics= 57625.25Hz	8bitoverflow=  225.220Hz	16bit=   0.880Hz
// 4 = CLOCK/256	tics= 14414.063Hz	8bitoverflow=   56.305Hz	16bit=   0.220Hz
// 5 = CLOCK/1024	tics=  3603.516Hz	8bitoverflow=   14.076Hz	16bit=   0.055Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 32.768KHz crystal on timer 2 (use for real-time clock)
// 0 = STOP
// 1 = CLOCK		tics= 32.768kHz		8bitoverflow= 128Hz
// 2 = CLOCK/8		tics= 4096kHz		8bitoverflow=  16Hz
// 3 = CLOCK/32		tics= 1024kHz		8bitoverflow=   4Hz
// 4 = CLOCK/64		tics= 512Hz			8bitoverflow=   2Hz
// 5 = CLOCK/128	tics= 256Hz			8bitoverflow=   1Hz
// 6 = CLOCK/256	tics= 128Hz			8bitoverflow=   0.5Hz
// 7 = CLOCK/1024	tics= 32Hz			8bitoverflow=   0.125Hz

#define TIMER_CLK_STOP			0x00	///< Timer Stopped
#define TIMER_CLK_DIV1			0x01	///< Timer clocked at F_CPU
#define TIMER_CLK_DIV8			0x02	///< Timer clocked at F_CPU/8
#define TIMER_CLK_DIV64			0x03	///< Timer clocked at F_CPU/64
#define TIMER_CLK_DIV256		0x04	///< Timer clocked at F_CPU/256
#define TIMER_CLK_DIV1024		0x05	///< Timer clocked at F_CPU/1024
#define TIMER_CLK_T_FALL		0x06	///< Timer clocked at T falling edge
#define TIMER_CLK_T_RISE		0x07	///< Timer clocked at T rising edge
#define TIMER_PRESCALE_MASK		0x07	///< Timer Prescaler Bit-Mask

#define TIMERRTC_CLK_STOP		0x00	///< RTC Timer Stopped
#define TIMERRTC_CLK_DIV1		0x01	///< RTC Timer clocked at F_CPU
#define TIMERRTC_CLK_DIV8		0x02	///< RTC Timer clocked at F_CPU/8
#define TIMERRTC_CLK_DIV32		0x03	///< RTC Timer clocked at F_CPU/32
#define TIMERRTC_CLK_DIV64		0x04	///< RTC Timer clocked at F_CPU/64
#define TIMERRTC_CLK_DIV128		0x05	///< RTC Timer clocked at F_CPU/128
#define TIMERRTC_CLK_DIV256		0x06	///< RTC Timer clocked at F_CPU/256
#define TIMERRTC_CLK_DIV1024	0x07	///< RTC Timer clocked at F_CPU/1024
#define TIMERRTC_PRESCALE_MASK	0x07	///< RTC Timer Prescaler Bit-Mask

// default prescale settings for the timers
// these settings are applied when you call
// timerInit or any of the timer<x>Init
#define TIMER0PRESCALE		TIMER_CLK_DIV1024		///< timer 0 prescaler default//original was TIMER_CLK_DIV8
#define TIMER1PRESCALE		TIMER_CLK_DIV64		///< timer 1 prescaler default
#define TIMER2PRESCALE		TIMERRTC_CLK_DIV64	///< timer 2 prescaler default
#define TIMER3PRESCALE		TIMER_CLK_DIV64		///< timer 3 prescaler default
#define TIMER4PRESCALE		TIMER_CLK_DIV64		///< timer 4 prescaler default
#define TIMER5PRESCALE		TIMER_CLK_DIV64		///< timer 4 prescaler default

// interrupt macros for attaching user functions to timer interrupts
// use these with timerAttach( intNum, function )
#define TIMER0OVERFLOW_INT			0

#define TIMER1OVERFLOW_INT			1
#define TIMER1OUTCOMPAREA_INT		2
#define TIMER1OUTCOMPAREB_INT		3
#define TIMER1INPUTCAPTURE_INT		4

#define TIMER2OVERFLOW_INT			5
#define TIMER2OUTCOMPARE_INT		6

#ifdef OCR0	// for processors that support output compare on Timer0
#define TIMER0OUTCOMPARE_INT		7
#define TIMER_NUM_INTERRUPTS		8
#else
#define TIMER_NUM_INTERRUPTS		7
#endif

//SoR Added
#define TIMER3OVERFLOW_INT			9
#define TIMER4OVERFLOW_INT			10
#define TIMER5OVERFLOW_INT			11
#define TIMER0OUTCOMPAREA_INT		12
#define TIMER0OUTCOMPAREB_INT		13
#define TIMER0INPUTCAPTURE_INT		14
#define TIMER2OUTCOMPAREA_INT		15
#define TIMER2OUTCOMPAREB_INT		16
#define TIMER2INPUTCAPTURE_INT		17

// default type of interrupt handler to use for timers
// *do not change unless you know what you're doing
// Value may be SIGNAL or INTERRUPT
#ifndef TIMER_INTERRUPT_HANDLER
#define TIMER_INTERRUPT_HANDLER		SIGNAL
#endif

// functions
#define delay		delay_us
#define delay_ms	timerPause
void delay_us(unsigned short time_us);

//! initializes timing system (all timers)
// runs all timer init functions
// sets all timers to default prescale values #defined in systimer.c
void timerInit(void);

// default initialization routines for each timer
void timer0Init(void);		///< initialize timer0
void timer1Init(void);		///< initialize timer1
#ifdef TCNT2	// support timer2 only if it exists
void timer2Init(void);		///< initialize timer2
#endif
#ifdef TCNT3	// support timer3 only if it exists
void timer3Init(void);		///< initialize timer3
#endif
#ifdef TCNT4	// support timer4 only if it exists
void timer4Init(void);		///< initialize timer4
#endif
#ifdef TCNT5	// support timer5 only if it exists
void timer5Init(void);		///< initialize timer4
#endif

// Clock prescaler set/get commands for each timer/counter
// For setting the prescaler, you should use one of the #defines
// above like TIMER_CLK_DIVx, where [x] is the division rate
// you want.
// When getting the current prescaler setting, the return value
// will be the [x] division value currently set.
void timer0SetPrescaler(u08 prescale);		///< set timer0 prescaler
u16  timer0GetPrescaler(void);				///< get timer0 prescaler
void timer1SetPrescaler(u08 prescale);		///< set timer1 prescaler
u16  timer1GetPrescaler(void);				///< get timer1 prescaler
#ifdef TCNT2	// support timer2 only if it exists
void timer2SetPrescaler(u08 prescale);		///< set timer2 prescaler
u16  timer2GetPrescaler(void);				///< get timer2 prescaler
#endif
#ifdef TCNT3	// support timer3 only if it exists
void timer3SetPrescaler(u08 prescale);		///< set timer3 prescaler
u16  timer3GetPrescaler(void);				///< get timer3 prescaler
#endif
#ifdef TCNT4	// support timer4 only if it exists
void timer4SetPrescaler(u08 prescale);		///< set timer4 prescaler
u16  timer4GetPrescaler(void);				///< get timer4 prescaler
#endif
#ifdef TCNT5	// support timer4 only if it exists
void timer5SetPrescaler(u08 prescale);		///< set timer4 prescaler
u16  timer5GetPrescaler(void);				///< get timer4 prescaler
#endif


// TimerAttach and Detach commands
//		These functions allow the attachment (or detachment) of any user function
//		to a timer interrupt.  "Attaching" one of your own functions to a timer
//		interrupt means that it will be called whenever that interrupt happens.
//		Using attach is better than rewriting the actual INTERRUPT() function
//		because your code will still work and be compatible if the timer library
//		is updated.  Also, using Attach allows your code and any predefined timer
//		code to work together and at the same time.  (ie. "attaching" your own
//		function to the timer0 overflow doesn't prevent timerPause from working,
//		but rather allows you to share the interrupt.)
//
//		timerAttach(TIMER1OVERFLOW_INT, myOverflowFunction);
//		timerDetach(TIMER1OVERFLOW_INT)
//
//		timerAttach causes the myOverflowFunction() to be attached, and therefore
//		execute, whenever an overflow on timer1 occurs.  timerDetach removes the
//		association and executes no user function when the interrupt occurs.
//		myOverflowFunction must be defined with no return value and no arguments:
//
//		void myOverflowFunction(void) { ... }

//! Attach a user function to a timer interrupt
void timerAttach(u08 interruptNum, void (*userFunc)(void) );
//! Detach a user function from a timer interrupt
void timerDetach(u08 interruptNum);


// timing commands
/// A timer-based delay/pause function
/// @param pause_ms	Number of integer milliseconds to wait.
void timerPause(unsigned short pause_ms);

// overflow counters
void timer0ClearOverflowCount(void);	///< Clear timer0's overflow counter. 
long timer0GetOverflowCount(void);		///< read timer0's overflow counter
void timer1ClearOverflowCount(void);	///< Clear timer1's overflow counter. 
long timer1GetOverflowCount(void);		///< read timer1's overflow counter
#ifdef TCNT2	// support timer2 only if it exists
void timer2ClearOverflowCount(void);	///< clear timer2's overflow counter
long timer2GetOverflowCount(void);		///< read timer2's overflow counter
#endif
#ifdef TCNT3	// support timer3 only if it exists
void timer3ClearOverflowCount(void);	///< clear timer3's overflow counter
long timer3GetOverflowCount(void);		///< read timer3's overflow counter
#endif
#ifdef TCNT4	// support timer4 only if it exists
void timer4ClearOverflowCount(void);	///< clear timer4's overflow counter
long timer4GetOverflowCount(void);		///< read timer4's overflow counter
#endif
#ifdef TCNT5	// support timer5 only if it exists
void timer5ClearOverflowCount(void);	///< clear timer4's overflow counter
long timer5GetOverflowCount(void);		///< read timer4's overflow counter
#endif


/*


/// Enter standard PWM Mode on timer0.
/// \param bitRes	indicates the period/resolution to use for PWM output in timer bits.
///						Must be either 8, 9, or 10 bits corresponding to PWM periods of 256, 512, or 1024 timer tics.
void timer0PWMInit(u08 bitRes);

/// Enter PWM Mode on timer0 with a specific top-count value.
/// \param topcount	indicates the desired PWM period in timer tics.
///						Can be a number between 1 and 65535 (16-bit).
void timer0PWMInitICR(u16 topcount);

/// Turn off all timer0 PWM output and set timer mode to normal.
void timer0PWMOff(void);

/// Turn on/off timer0 PWM outputs.
void timer0PWMAOn(void);			///< Turn on timer0 Channel A PWM output
void timer0PWMBOn(void);			///< Turn on timer0 Channel B PWM output
void timer0PWMCOn(void);			///< Turn on timer0 Channel C PWM output
void timer0PWMAOff(void);			///< turn off timer0 Channel A PWM output
void timer0PWMBOff(void);			///< turn off timer0 Channel B PWM output
void timer0PWMCOff(void);			///< turn off timer0 Channel C PWM output

void timer0PWMASet(u16 pwmDuty);	///< set duty of timer0 Channel A PWM output
void timer0PWMBSet(u16 pwmDuty);	///< set duty of timer0 Channel B PWM output
void timer0PWMCSet(u16 pwmDuty);	///< set duty of timer0 Channel C PWM output




/// Enter standard PWM Mode on timer1.
/// \param bitRes	indicates the period/resolution to use for PWM output in timer bits.
///						Must be either 8, 9, or 10 bits corresponding to PWM periods of 256, 512, or 1024 timer tics.
void timer1PWMInit(u08 bitRes);

/// Enter PWM Mode on timer1 with a specific top-count value.
/// \param topcount	indicates the desired PWM period in timer tics.
///						Can be a number between 1 and 65535 (16-bit).
void timer1PWMInitICR(u16 topcount);

/// Turn off all timer1 PWM output and set timer mode to normal.
void timer1PWMOff(void);

/// Turn on/off Timer1 PWM outputs.
void timer1PWMAOn(void);			///< Turn on timer1 Channel A PWM output
void timer1PWMBOn(void);			///< Turn on timer1 Channel B PWM output
void timer1PWMCOn(void);			///< Turn on timer1 Channel C PWM output
void timer1PWMAOff(void);			///< turn off timer1 Channel A PWM output
void timer1PWMBOff(void);			///< turn off timer1 Channel B PWM output
void timer1PWMCOff(void);			///< turn off timer1 Channel C PWM output

void timer1PWMASet(u16 pwmDuty);	///< set duty of timer1 Channel A PWM output
void timer1PWMBSet(u16 pwmDuty);	///< set duty of timer1 Channel B PWM output
void timer1PWMCSet(u16 pwmDuty);	///< set duty of timer1 Channel C PWM output

*/

/// Enter standard PWM Mode
/// \param bitRes	indicates the period/resolution to use for PWM output in timer bits.
///						Must be either 8, 9, or 10 bits corresponding to PWM periods of 256, 512, or 1024 timer tics.
void PWM_Init_timer1_LED(u08 bitRes);
void PWM_Init_timer2_H6(u08 bitRes);
void PWM_Init_timer3_E3(u08 bitRes);
void PWM_Init_timer3_E4(u08 bitRes);
void PWM_Init_timer3_E5(u08 bitRes);
void PWM_Init_timer4_H3(u08 bitRes);
void PWM_Init_timer4_H4(u08 bitRes);
void PWM_Init_timer4_H5(u08 bitRes);

/// Enter PWM Mode on timer2 with a specific top-count value.
/// \param topcount	indicates the desired PWM period in timer tics.
///						Can be a number between 1 and 65535 (16-bit).
void timer1PWMInitICR(u16 topcount);
void timer2PWMInitICR(u16 topcount);
void timer3PWMInitICR(u16 topcount);
void timer4PWMInitICR(u16 topcount);

/// Turn off all PWM output and set timer mode to normal.
void PWM_timer1_Off_All(void);
void PWM_timer2_Off_All(void);
void PWM_timer3_Off_All(void);
void PWM_timer4_Off_All(void);

/// Turn on PWM outputs.
void PWM_timer1_On_LED(void);
void PWM_timer2_On_H6(void);
void PWM_timer3_On_E3(void);
void PWM_timer3_On_E4(void);
void PWM_timer3_On_E5(void);
void PWM_timer4_On_H3(void);
void PWM_timer4_On_H4(void);
void PWM_timer4_On_H5(void);

/// Turn on PWM outputs.
void PWM_timer1_Off_LED(void);
void PWM_timer2_Off_H6(void);
void PWM_timer3_Off_E3(void);
void PWM_timer3_Off_E4(void);
void PWM_timer3_Off_E5(void);
void PWM_timer4_Off_H3(void);
void PWM_timer4_Off_H4(void);
void PWM_timer4_Off_H5(void);

///< set duty of timer2 PWM output
void PWM_timer1_Set_LED(u16 pwmDuty);
void PWM_timer2_Set_H6(u16 pwmDuty);
void PWM_timer3_Set_E3(u16 pwmDuty);
void PWM_timer3_Set_E4(u16 pwmDuty);
void PWM_timer3_Set_E5(u16 pwmDuty);
void PWM_timer4_Set_H3(u16 pwmDuty);
void PWM_timer4_Set_H4(u16 pwmDuty);
void PWM_timer4_Set_H5(u16 pwmDuty);	



#endif
