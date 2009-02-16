//AXON test program
//sends highspeed pulses to each pin to be read by an oscope
//all ADC are turned on, and all ADC data is transmitted by USB
//all other UARTs are transmitting

void test_oscope(void)
	{
	int se0,se1,se2,se3,se4,se5,se6,se7,se8;
	int se9,se10,se11,se12,se13,se14,se15;



	LED_on();

	if (button_pressed())
		LED_off();


	//pulse each fast for oscope testing
	//right row
	servo(PORTC,0,360);
	servo(PORTC,1,360);
	servo(PORTC,2,360);
	servo(PORTC,3,360);
	servo(PORTC,4,360);
	servo(PORTC,5,360);
	servo(PORTC,6,360);
	servo(PORTC,7,360);

	servo(PORTJ,6,360);
	
	servo(PORTA,0,360);
	servo(PORTA,1,360);
	servo(PORTA,2,360);
	servo(PORTA,3,360);
	servo(PORTA,4,360);
	servo(PORTA,5,360);
	servo(PORTA,6,360);
	servo(PORTA,7,360);
	
	//left row
	servo(PORTE,2,360);
	servo(PORTE,3,360);
	servo(PORTE,4,360);
	servo(PORTE,5,360);
	servo(PORTE,6,360);
	servo(PORTE,7,360);
	
	servo(PORTH,2,360);
	servo(PORTH,3,360);
	servo(PORTH,4,360);
	servo(PORTH,5,360);
	servo(PORTH,6,360);


	//top row
	se0=a2dConvert8bit(0);
	se1=a2dConvert8bit(1);
	se2=a2dConvert8bit(2);
	se3=a2dConvert8bit(3);
	se4=a2dConvert8bit(4);
	se5=a2dConvert8bit(5);
	se6=a2dConvert8bit(6);
	se7=a2dConvert8bit(7);
	se8=a2dConvert8bit(8);
	se9=a2dConvert8bit(9);
	se10=a2dConvert8bit(10);
	se11=a2dConvert8bit(11);
	se12=a2dConvert8bit(12);
	se13=a2dConvert8bit(13);
	se14=a2dConvert8bit(14);
	se15=a2dConvert8bit(0x0f);//15


	rprintfInit(uart1SendByte);
	rprintf("0:%d 1:%d 2:%d 3:%d 4:%d 5:%d 6:%d 7:%d \r\n",se0,se1,se2,se3,se4,se5,se6,se7);
	rprintf("8:%d 9:%d 10:%d 11:%d 12:%d 13:%d 14:%d 15:%d \r\n",se8,se9,se10,se11,se12,se13,se14,se15);

	rprintfInit(uart0SendByte);
	rprintf(" 0");
	rprintfInit(uart2SendByte);
	rprintf(" 2");
	rprintfInit(uart3SendByte);
	rprintf(" 3");


	delay_cycles(65500);
	}
