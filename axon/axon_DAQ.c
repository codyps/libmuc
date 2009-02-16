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

//this file contains code to convert the Axon into a DAQ (data acquisition device)


void axon_DAQ(void)
	{
	int a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15;

	//select speed/accuracy of data conversion
	a2dSetPrescaler(ADC_PRESCALE_DIV32); // configure ADC scaling

	// A2D clock prescaler select
	//		*selects how much the CPU clock frequency is divided
	//		to create the A2D clock frequency
	//		*lower division ratios make conversion go faster
	//		*higher division ratios make conversions more accurate
	// ADC_PRESCALE_DIV2 		-> CPU clk/2
	// ADC_PRESCALE_DIV4	  	-> CPU clk/4
	// ADC_PRESCALE_DIV8	  	-> CPU clk/8
	// ADC_PRESCALE_DIV16		-> CPU clk/16
	// ADC_PRESCALE_DIV32		-> CPU clk/32
	// ADC_PRESCALE_DIV64		-> CPU clk/64
	// ADC_PRESCALE_DIV128		-> CPU clk/128

	while(1)
		{
		//clear timer
		reset_timer_2();

		//if you don't require high speed data collection,
		//add a delay here to slow it down
		//be careful that clock doesn't overflow!
		//delay_ms(1);

		//gather data
		a0=a2dConvert8bit(0);
		a1=a2dConvert8bit(1);
		a2=a2dConvert8bit(2);
		a3=a2dConvert8bit(3);
		a4=a2dConvert8bit(4);
		a5=a2dConvert8bit(5);
		a6=a2dConvert8bit(6);
		a7=a2dConvert8bit(7);
		a8=a2dConvert8bit(8);
		a9=a2dConvert8bit(9);
		a10=a2dConvert8bit(10);
		a11=a2dConvert8bit(11);
		a12=a2dConvert8bit(12);
		a13=a2dConvert8bit(13);
		a14=a2dConvert8bit(14);
		a15=a2dConvert8bit(15);

		//report data, add time stamp and overflow count at end of data
		rprintf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\r\n",a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,(timer2GetOverflowCount()*255+TCNT2));
		}
	}


