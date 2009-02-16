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

//this file contains code to use with various robot sensors

/***************SHARP IR*******************/
//These functions convert ADC 8 bit values to distances in centimeters
//Maximum error is about 10%
//
//to learn more about Sharp IR, please visit:
//http://www.societyofrobots.com/sensors_sharpirrange.shtml
//
//want to post your calibration data?
//visit here: http://www.societyofrobots.com/robotforum/index.php?topic=4616.0
//
//you can power this sensor directly from the Axon's ADC header row


//Sharp GP2D12 IR Range Sensor -  claims 10cm to 80cm (I got 8cm to 150cm)
int sharp_IR_interpret_GP2D12(int value)
	{
	return 1384.4*pow(value,-.9988);
	}
//Sharp GP2Y0A02YK IR Range Sensor - 20 cm to 150 cm
int sharp_IR_interpret_GP2Y0A02YK(int value)
	{
	return 1904.5*pow(value,-.89);
	}
//Sharp GP2Y0A21YK IR Range Sensor - 4 cm to 30 cm
int sharp_IR_interpret_GP2Y0A21YK(int value)
	{
	return 739.38*pow(value,-.8105);
	}
//Sharp GP2D120 IR Range Sensor - 4 to 30 cm 
int sharp_IR_interpret_GP2D120(int value)
	{
	return 509.88*pow(value,-.9154);
	}
//Sharp GP2D15 IR Range Sensor - 10cm to 80cm  
int sharp_IR_interpret_GP2D15(int value)
	{
	return 736.21*pow(value,-.7922);
	}



/************ACCELEROMETER****************/
//ACCM3D for more info, see:
//http://www.dimensionengineering.com/DE-ACCM3D.htm
//you can power this sensor directly from the Axon's 5V ADC header row
//converts ADC value to degrees
int accelerometer_ACCM3D(int value)
	{
	return (8.1818*value-8.1818);
	}

//SEN-00741 for more info, see:
//http://www.sparkfun.com/commerce/product_info.php?products_id=741
//http://www.sparkfun.com/datasheets/Components/ADXL330_0.pdf
//use the 3.3V output on the Axon to power this sensor (giving it 5V will fry it!)
int accelerometer_SEN00741(int value)
	{
	return (8.1818*value-8.1818);
	}



/*******************GYRO******************/
//SEN-00741 for more info, see:
//http://www.sparkfun.com/commerce/product_info.php?products_id=741
//http://www.sparkfun.com/datasheets/Components/IDG-300_Datasheet.pdf
//use the 3.3V output on the Axon to power this sensor (giving it 5V will fry it!)
//converts ADC value to degrees
//2 mV/deg/second, effectively linear at 1%
int gyro_SEN00741(int value)
	{
	return (15*value-180);
	}



/******************SONAR******************/
//LV_MaxSonar_EZ1: http://www.maxbotix.com/uploads/LV-MaxSonar-EZ1-Datasheet.pdf
//you can power these sensors directly from the Axon's 5V ADC header row
//A supply of 5V yields ~9.8mV/in for all EZ versions
int sonar_MaxSonar(int value)
	{
	return (50.01*value-51.02);
	}

//PING Sonar
//equations were taken from here and not independently verified:
//http://www.societyofrobots.com/member_tutorials/node/174
//http://www.societyofrobots.com/robotforum/index.php?topic=5123.msg40008
//http://www.societyofrobots.com/robotforum/index.php?topic=4656.30
//uses timer0
int sonar_Ping(void)
	{
	#define PINGPIN    3          // assign a pin to the Ping Sensor
	#define DDR        DDRA
	#define PORT       PORTA
	#define PIN        PINA

	PORT_ON(DDR, PINGPIN);   // Switch PingPin to OUPUT
	// ------Trigger Pulse--------------
	PORT_OFF(PORT, PINGPIN);   // Bring PingPin low before starting trigger pulse
	delay_us(2);        //  Wait for 2 microseconds
	PORT_ON(PORT, PINGPIN);    // Bring PingPin High for 5us according to spec sheet.
	delay_us(5);       // Wait for 5 microseconds
	PORT_OFF(PORT, PINGPIN);; //  Bring PingPin Low and standby
	//--------End Trigger Pulse---------------------
	FLIP_PORT(DDR, PINGPIN);   // Switch PingPin to INPUT
	loop_until_bit_is_set(PIN, PINGPIN);     // Loop until the the PingPin goes high  (macro found in sfr_def.h)
	//clears timer, reset overflow counter
	reset_timer_0();       //reset timer 0
	loop_until_bit_is_clear(PIN, PINGPIN);     // Loop until the the PingPin goes low  (macro found in sfr_def.h)
	//read timer0's overflow counter
	//255 is count before overflow, dependent on clock
	
	return (timer0GetOverflowCount()*255+TCNT0) * 2.068965517;//elapsed time x conversion
	}

/*****************Phidgets****************/
//equations were taken from here and not independently verified:
//http://www.robotshop.us/PDF/Phidgets-Sensors.pdf
//you can power these sensors directly from the Axon's 5V ADC header row
//(unless otherwise specified)
//Gas Pressure Sensor
int pressure_phidget(int value)
	{
	return (value*0.25 + 10);//returns in kilopascals
	}
//Voltage Sensor
int voltage_phidget(int value)
	{
	return ((value - 500) * 0.06);//returns in volts
	}
//Magnetic Sensor
int magnetic_phidget(int value)
	{
	return (500 - value);//returns in gauss
	}
//Temperature Sensor
int temperature_phidget(int value)
	{
	return ((value-200)/4);//returns in celsius
	}
//50A Current Sensor
int currentDC50_phidget(int value)
	{
	return ((value-500)* 0.125);//returns in amps
	}
//50A Current Sensor
int currentAC50_phidget(int value)
	{
	return (value*0.0625);//returns in amps
	}
//20A Current Sensor
int currentDC20_phidget(int value)
	{
	return ((value-500)* 0.05);//returns in amps
	}
//20A Current Sensor
int currentAC20_phidget(int value)
	{
	return (value*0.025);//returns in amps
	}
//Humidity Sensor
int humidity_phidget(int value)
	{
	return ((value*0.1946) - 41.98);//returns in RH (in %)
	}





/************DIGITAL COMPASS - INCOMPLETE & UNTESTED**************/
//CMPS03 - Robot Compass Module
//for more info, see:
//http://www.robot-electronics.co.uk/htm/cmps3doc.shtml
//calibration instructions: http://www.robot-electronics.co.uk/htm/cmps_cal.shtml
//you can power this sensor directly from the Axon's 5V ADC header row
//PWM mode using pin A0 - this pin is NOT voltage regulated!
/*int compass_CMPS03(int value)
	{
	int time;

	cbi(DDRA, PA0);//set port as digital input

	if(bit_is_set(PINA, 0)) //if PWM is high
		while(bit_is_set(PINA, 0)); //wait until it goes low

	while(bit_is_clear(PINA, 0));//wait while PWM is low

	//PWM now went high, so start timer
	reset_timer_2();

	while(bit_is_set(PINA, 0)); //wait until it goes low again

	time=timer2GetOverflowCount()*255+TCNT2;//store time passed

	}
*/
