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


//this file contains code to use with various robot hardware

/***************SABERTOOTH****************/
//This is the motor driver by dimensionengineering. This
//program lets you send serial data to the driver for motor control.
//datasheets:
//http://www.dimensionengineering.com/datasheets/Sabertooth2x5.pdf
//http://www.dimensionengineering.com/datasheets/Sabertooth2x25.pdf
//http://www.dimensionengineering.com/datasheets/Sabertooth2x10.pdf
//note that instead of using serial, you can also use PWM or servo command code

//see page 13 for serial information
//Sending a character between 1 and 127 will control motor 1
//1 is full reverse, 64 is stop and 127 is full forward

//Sending a character between 128 and 255 will control motor 2
//128 is full reverse, 192 is stop and 255 is full forward

//Character 0 will shut down both motors

//example: sabertooth(38,200);//send command to motors

void sabertooth(int m1, int m2)
	{
	//rprintfInit(uart2SendByte);//select uart2
	uartSetBaudRate(2, 38400);//select baud (see page 13/14 of datasheet)
	uart2SendByte(m1);//send a command for motor 1
	uart2SendByte(m2);//send a command for motor 2
	}

/*********************SYREN*******************/
//This is the motor driver by dimensionengineering. This
//program lets you send serial data to the driver for motor control.
//note that instead of using serial, you can also use PWM or servo command code

//http://www.dimensionengineering.com/datasheets/SyRen10-20.pdf
//http://www.dimensionengineering.com/datasheets/SyRen10-25.pdf

//0 represents full reverse and 255 represents full forward

void syren(int m1)
	{
	//rprintfInit(uart2SendByte);//select uart2
	uartSetBaudRate(2, 38400);//select baud (see page 14 of datasheet)
	uart2SendByte(m1);//send a command for motor 1
	}




/**********************HD44780 LCD******************/
//code can be found here:
//http://homepage.hispeed.ch/peterfleury/lcdlibrary.zip
//example:
//http://www.societyofrobots.com/robotforum/index.php?topic=6175



/**********TReX dual motor controller DMC01 **********/
//http://www.pololu.com/catalog/product/777/resources
//TReX ships with a default baud rate of 19,200 bps
//documentation too complicated for my laziness to figure it out, any volunteers?
/*
int trex(int m1)
	{
	rprintfInit(uart2SendByte);//select uart2
	uartSetBaudRate(2, 19200);//select baud, 19.2kbps is default for TReX
	rprintf("%d", m1);//send a command for motor 1
	}
*/
