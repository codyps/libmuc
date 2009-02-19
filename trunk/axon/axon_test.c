//AXON test program
//Pulses all pins for 1 second
//sends data to all UARTs

void test(void)
	{

	LED_on();

	if (button_pressed())
		LED_off();

	DDRF = 0b11111111;  //configure all F ports for output
	DDRK = 0b11111111;  //configure all K ports for output

	//left row
	PORT_ON(PORTH,6);
	PORT_ON(PORTH,5);
	PORT_ON(PORTH,4);
	PORT_ON(PORTH,3);
	PORT_ON(PORTH,2);
	PORT_ON(PORTE,7);
	PORT_ON(PORTE,6);
	PORT_ON(PORTE,5);
	PORT_ON(PORTE,4);
	PORT_ON(PORTE,3);
	PORT_ON(PORTE,2);

	//right row
	PORT_ON(PORTC,0);
	PORT_ON(PORTC,1);
	PORT_ON(PORTC,2);
	PORT_ON(PORTC,3);
	PORT_ON(PORTC,4);
	PORT_ON(PORTC,5);
	PORT_ON(PORTC,6);
	PORT_ON(PORTC,7);
	PORT_ON(PORTJ,6);
	PORT_ON(PORTA,7);
	PORT_ON(PORTA,6);
	PORT_ON(PORTA,5);
	PORT_ON(PORTA,4);
	PORT_ON(PORTA,3);
	PORT_ON(PORTA,2);
	PORT_ON(PORTA,1);
	PORT_ON(PORTA,0);

	//top row
	PORT_ON(PORTF,0);
	PORT_ON(PORTF,1);
	PORT_ON(PORTF,2);
	PORT_ON(PORTF,3);
	PORT_ON(PORTF,4);
	PORT_ON(PORTF,5);
	PORT_ON(PORTF,6);
	PORT_ON(PORTF,7);
	PORT_ON(PORTK,0);
	PORT_ON(PORTK,1);
	PORT_ON(PORTK,2);
	PORT_ON(PORTK,3);
	PORT_ON(PORTK,4);
	PORT_ON(PORTK,5);
	PORT_ON(PORTK,6);
	PORT_ON(PORTK,7);


	
	delay_ms(800);//delay


	rprintfInit(uart0SendByte);
	rprintf(" 0");
	rprintfInit(uart1SendByte);
	rprintf(" 1");
	rprintfInit(uart2SendByte);
	rprintf(" 2");
	rprintfInit(uart3SendByte);
	rprintf(" 3");


	//left row
	PORT_OFF(PORTH,6);
	PORT_OFF(PORTH,5);
	PORT_OFF(PORTH,4);
	PORT_OFF(PORTH,3);
	PORT_OFF(PORTH,2);
	PORT_OFF(PORTE,7);
	PORT_OFF(PORTE,6);
	PORT_OFF(PORTE,5);
	PORT_OFF(PORTE,4);
	PORT_OFF(PORTE,3);
	PORT_OFF(PORTE,2);

	//right row
	PORT_OFF(PORTC,0);
	PORT_OFF(PORTC,1);
	PORT_OFF(PORTC,2);
	PORT_OFF(PORTC,3);
	PORT_OFF(PORTC,4);
	PORT_OFF(PORTC,5);
	PORT_OFF(PORTC,6);
	PORT_OFF(PORTC,7);
	PORT_OFF(PORTJ,6);
	PORT_OFF(PORTA,7);
	PORT_OFF(PORTA,6);
	PORT_OFF(PORTA,5);
	PORT_OFF(PORTA,4);
	PORT_OFF(PORTA,3);
	PORT_OFF(PORTA,2);
	PORT_OFF(PORTA,1);
	PORT_OFF(PORTA,0);

	//top row
	PORT_OFF(PORTF,0);
	PORT_OFF(PORTF,1);
	PORT_OFF(PORTF,2);
	PORT_OFF(PORTF,3);
	PORT_OFF(PORTF,4);
	PORT_OFF(PORTF,5);
	PORT_OFF(PORTF,6);
	PORT_OFF(PORTF,7);
	PORT_OFF(PORTK,0);
	PORT_OFF(PORTK,1);
	PORT_OFF(PORTK,2);
	PORT_OFF(PORTK,3);
	PORT_OFF(PORTK,4);
	PORT_OFF(PORTK,5);
	PORT_OFF(PORTK,6);
	PORT_OFF(PORTK,7);

	delay_ms(800);//delay


	rprintfInit(uart0SendByte);
	rprintf(" port");
	rprintfInit(uart1SendByte);
	rprintf(" port");
	rprintfInit(uart2SendByte);
	rprintf(" port");
	rprintfInit(uart3SendByte);
	rprintf(" port");


	delay_cycles(65500);
	}
