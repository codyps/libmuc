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

//This sourcecode is for the Axon servo controller
//http://www.societyofrobots.com/robotforum/index.php?topic=4886.0

char buffer[30];//stores a command
unsigned int buffer_count=0;//keeps track of newest buffer value
//stores angles of each servo, initialize at servo centers
int  servo_list[27]={110,110,110,110,110,110,110,110,110,
					 110,110,110,110,110,110,110,110,110,
					 110,110,110,110,110,110,110,110,110};

void get_data(void);

/*
When interrupt-driven data reception is used, the receive complete routine must read
the received data from UDR in order to clear the RXC Flag, otherwise a new interrupt
will occur once the interrupt routine terminates.
*/
ISR(USART_RX_vect) //trigger interrupt when uart1 receives data
	{
	//Code to be executed when the USART receives a byte here
	//ReceivedByte = UDR1; // Fetch the recieved byte value into the variable "ByteReceived", resets interrupt
	//UDR1 = ReceivedByte; // Echo back the received byte back to the computer, use for testing

	cli();//turn off interrupts
	get_data();//exit interrupt and get data
	}

void servo_controller(void)
	{
	int i;
	int multiplier=5;

	//initialize ISR:
	UCSR1B |= (1 << RXCIE1); // Enable the USART Recieve Complete interrupt (USART_RXCn)
	sei();//turn on the Global Interrupt Enable flag
	//cli();//turn off interrupts

	while(1)
		{//command servos
		for(i=0;i<27;i++)
			{
			servo(PORTH,6,servo_list[i]*multiplier);
			servo(PORTH,5,servo_list[i]*multiplier);
			servo(PORTH,4,servo_list[i]*multiplier);
			servo(PORTH,3,servo_list[i]*multiplier);
			servo(PORTH,2,servo_list[i]*multiplier);
			servo(PORTE,7,servo_list[i]*multiplier);
			servo(PORTE,6,servo_list[i]*multiplier);
			servo(PORTE,5,servo_list[i]*multiplier);
			servo(PORTE,4,servo_list[i]*multiplier);
			servo(PORTE,3,servo_list[i]*multiplier);
			servo(PORTE,2,servo_list[i]*multiplier);
			servo(PORTA,0,servo_list[i]*multiplier);
			servo(PORTA,1,servo_list[i]*multiplier);
			servo(PORTA,2,servo_list[i]*multiplier);
			servo(PORTA,3,servo_list[i]*multiplier);
			servo(PORTA,4,servo_list[i]*multiplier);
			servo(PORTA,5,servo_list[i]*multiplier);
			servo(PORTA,6,servo_list[i]*multiplier);
			servo(PORTA,7,servo_list[i]*multiplier);
			servo(PORTJ,6,servo_list[i]*multiplier);
			servo(PORTC,7,servo_list[i]*multiplier);
			servo(PORTC,6,servo_list[i]*multiplier);
			servo(PORTC,5,servo_list[i]*multiplier);
			servo(PORTC,4,servo_list[i]*multiplier);
			servo(PORTC,3,servo_list[i]*multiplier);
			servo(PORTC,2,servo_list[i]*multiplier);
			servo(PORTC,1,servo_list[i]*multiplier);
			servo(PORTC,0,servo_list[i]*multiplier);

			//add timer delay here to ensure a min of 20 ms
			}
		}
	}

void get_data(void)
	{
	int temp;
	int counter;

	LED_on();

	while(counter<27 || (temp & 0x0F) == 'b')//wait until the entire stream is received, or exit character recieved
		{
		temp=uart1GetByte();//returns -1 if no data present
	
	  	if (temp != -1)//if data received
			{
			rprintf("%c",temp);
			servo_list[counter]=temp & 0x0F;//store values into an array, &0x0F strips ascii off to use in char strings
			counter++;
			}
		if ((temp & 0x0F) == 'a')//if reset character is detected
			counter=0;//reset
		}
	
	uartFlushReceiveBuffer(1);//flush out receive buffer

	LED_off();

	sei();//turn on interrupts

	//goes back to where originally left off
	}
