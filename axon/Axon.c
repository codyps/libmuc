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

//SoR Include
#include "SoR_Utils.h" //includes all the technical stuff
#include "hardware.c" //declare hardware variables and ports
//#include "CMUcam.c" //not yet written
#include "sensors.c" //sensor libraries for sonar, sharp IR, etc.
#include "misc.c" //includes libraries for various hardware and other useful stuff
//#include "axon_DAQ.c" //use the Axon like a data acquisition device
//#include "Blackfin_Axon.c" //files for Blackfin Robot camera
#include "control.c" //your code goes in here
//#include "servo_controller.c" //Axon servo controller
//#include "axon_test.c" //include this is doing a function test for the Axon
//#include "axon_oscope_test.c" //include this is doing a function test for the Axon


int main(void)
	{
	//declare variables here
	int i=0;//useless variable
	int j=0;//useless variable


	/****************INITIALIZATIONS*******************/
	//other stuff Im experimenting with for SoR
	uartInit();  // initialize the UART (serial port)
    uartSetBaudRate(0, 38400); // set UARTE speed, for Bluetooth
    uartSetBaudRate(1, 115200); // set UARTD speed, for USB connection
    uartSetBaudRate(2, 38400); // set UARTH speed
    uartSetBaudRate(3, 38400); // set UARTJ speed, for Blackfin
	//G=Ground, T=Tx (connect to external Rx), R=Rx (connect to external Tx)

	rprintfInit(uart1SendByte);// initialize rprintf system and configure uart1 (USB) for rprintf

	configure_ports(); // configure which ports are analog, digital, etc.

	LED_on();

	rprintf("\r\nSystem Warming Up");

	// initialize the timer system (comment out ones you don't want)
	timer0Init();
	timer1Init();
	timer2Init();
	timer3Init();
	timer4Init();
	//timer5Init();

	a2dInit(); // initialize analog to digital converter (ADC)
	a2dSetPrescaler(ADC_PRESCALE_DIV32); // configure ADC scaling
	a2dSetReference(ADC_REFERENCE_AVCC); // configure ADC reference voltage

	//let system stabelize for X time
	for(i=0;i<16;i++)
		{
		j=a2dConvert8bit(i);//read each ADC once to get it working accurately
		delay_cycles(5000); //keep LED on long enough to see Axon reseting
		rprintf(".");
		}

	LED_off();
	/**************************************************/

//test programs
//while(1)
//{
//test_oscope();
//test();
//}
	
	/*********ADD YOUR CODE BELOW THIS LINE **********/

	rprintf("Initialization Complete \r\n");

	//wait until user pushes button
	//while(!button_pressed());

	//reset all timers to zero
	reset_timer_0();
	reset_timer_1();
	reset_timer_2();
	reset_timer_3();
	reset_timer_4();
	//reset_timer_5();

	while(1)
		{
		control();//uncomment this for your code (and use control.c only to program)

		//servo_controller();

//below is various test code for Admin
/*
PWM_Init_timer1_LED(8);
PWM_timer1_On_LED();
while(1)
	{
	for(i=90;i<255;i++)
		{
		PWM_timer1_Set_LED(i);
		delay_ms(10);
		}
	for(i=255;i>90;i--)
		{
		PWM_timer1_Set_LED(i);
		delay_ms(10);
		}
		rprintf("stuff\r\n");
	}

rprintf("Initializing timer2 for PWM\r\n");
PWM_Init_timer1_LED(8);
PWM_Init_timer2_H6(8);//9 doesn't work
PWM_Init_timer3_E3(8);
PWM_Init_timer3_E4(8);
PWM_Init_timer3_E5(8);
delay_ms(100);
rprintf("2");
//PWM_Init_timer4_H3(10);
delay_ms(100);
rprintf("3");
//PWM_Init_timer4_H4(10);
delay_ms(100);
rprintf("4");
//PWM_Init_timer4_H5(10);
delay_ms(100);
rprintf("5");

rprintf("Turning on both PWM channels\r\n");
PWM_timer1_On_LED();
PWM_timer2_On_H6();
PWM_timer3_On_E3();
PWM_timer3_On_E4();
PWM_timer3_On_E5();
PWM_timer4_On_H3();
PWM_timer4_On_H4();
PWM_timer4_On_H5();
delay_ms(1000);

rprintf("Setting PWM to 1%% duty cycle\r\n");
PWM_timer1_Set_LED(1);
PWM_timer2_Set_H6(1);
PWM_timer3_Set_E3(1);
PWM_timer3_Set_E4(1);
PWM_timer3_Set_E5(1);
PWM_timer4_Set_H3(1);
PWM_timer4_Set_H4(1);
PWM_timer4_Set_H5(1);
delay_ms(1000); 

rprintf("Setting PWM to 50%% duty cycle\r\n");
PWM_timer1_Set_LED(127);
PWM_timer2_Set_H6(127);
PWM_timer3_Set_E3(127);
PWM_timer3_Set_E4(127);
PWM_timer3_Set_E5(127);
PWM_timer4_Set_H3(127);
PWM_timer4_Set_H4(127);
PWM_timer4_Set_H5(127);
delay_ms(1000); 

rprintf("Setting PWM to 99%% duty cycle\r\n");
PWM_timer1_Set_LED(254);
PWM_timer2_Set_H6(254);
PWM_timer3_Set_E3(254);
PWM_timer3_Set_E4(254);
PWM_timer3_Set_E5(254);
PWM_timer4_Set_H3(254);
PWM_timer4_Set_H4(254);
PWM_timer4_Set_H5(254);
delay_ms(1000);

rprintf("Turning off PWM\r\n");
PWM_timer1_Off_LED();
PWM_timer2_Off_H6();
PWM_timer3_Off_E3();
PWM_timer3_Off_E4();
PWM_timer3_Off_E5();
PWM_timer4_Off_H3();
PWM_timer4_Off_H4();
PWM_timer4_Off_H5();
delay_ms(1000);
*/
		delay_cycles(100);//an optional small delay to prevent crazy oscillations
		}
	/*********ADD YOUR CODE ABOVE THIS LINE **********/

	return 0;
	}
