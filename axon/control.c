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

//Add your code here

unsigned int sensor_left=128;
unsigned int sensor_right=128;
int auto_calib_L=0;
int auto_calib_R=0;

void update_sensors(void);
void photovore(void);
void test_code(void);

//enter your code in control here
//photovore is just a default sample program and can be deleted
void control(void)
	{
	photovore();//run photovore algorithm, or comment out for your own code









	//axon_DAQ();//activate the DAQ software (data acquisition)
	}

void update_sensors(void)
	{
	sensor_right=a2dConvert8bit(2)+auto_calib_R;
	sensor_left=a2dConvert8bit(14)+auto_calib_L;//right sensor is off, so I fudged it . . . mmmmm fudge!
	}

void photovore(void)
	{
	//initialize values
	long unsigned int wheel_left=500;
	long unsigned int wheel_right=500;
	unsigned int threshold = 8;

	//autocalibrate makes both start at equal values (so must start in same lighting)
	update_sensors();
	if (sensor_left>sensor_right)
		auto_calib_R=sensor_left-sensor_right;
	else
		auto_calib_L=sensor_right-sensor_left;
		

	while(1)
		{
		//get sensor data
		update_sensors();

		//detects more light on left side of robot
		if(sensor_left > sensor_right && (sensor_left - sensor_right) > threshold)
			{//go left
	 		wheel_left=550;
	 		wheel_right=550;
	 		}

	 	//detects more light on right side of robot
	 	else if(sensor_right > sensor_left && (sensor_right - sensor_left) > threshold)
			{//go right
			wheel_left=850;
			wheel_right=850;
			}

	 	//light is about equal on both sides
		else
			{//go straight
			wheel_left=600;
			wheel_right=800;
	 		}
 
		//output data to USB (use hyperterminal to view it)
		rprintf("L_Sensor=%d, L_wheel=%d%d, R_Sensor=%d, R_wheel=%d%d\r\n",sensor_left, wheel_left, sensor_right, wheel_right);

		//command servos
		wheel_left(wheel_left);
		wheel_right(wheel_right);
		}
	}
