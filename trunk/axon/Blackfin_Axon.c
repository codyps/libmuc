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
*	more commands here: http://www.surveyor.com/blackfin/SRV_protocol_bf.html
*	full tutorial here: http://www.societyofrobots.com/electronics_blackfin_camera.shtml
*
****************************************************************************/


char response[100];
int Y_mean, U_mean, V_mean;
int R_mean, B_mean, G_mean;
int bin;

//if these are made as long ints, rprintf blows up!
 int Y_top, Y_bottom, X_left, X_right;
 int X_centroid, Y_centroid;
 int X_old=40, Y_old=32;//set old values to central camera location
long int pixels;
long int cam_X_location;
long int cam_Y_location;

//Receive Repeat Response data from Blackfin, store into an array of size response_counter
void Blackfin_Echo(int response_counter) //echos # of characters expected for each command
	{
	int temp;
	int counter=0;

	rprintfInit(uart0SendByte);//change UART to bluetooth

	while(1)
		{
		temp=uart3GetByte();//returns -1 if no data present
		//rprintf("trapped!");

  	    if (temp != -1)//if data received
			{
			rprintf("%c",temp);
			response[counter]=temp & 0x0F;//store values into an array, &0x0F strips ascii off to use in char strings

			counter++;
	  	    if(counter >= response_counter || temp == '\n')
				{
				rprintf("\r\n\r\n");
				uartFlushReceiveBuffer(3);//flush out receive camera buffer to stop phase shifting
				break;
				}
			}
		}
	}


//Change Resolution
//send an 'a' command to change resolution to 80x64 (x/y) (you should receive an '#a' ack)
//160x128 for later
void Blackfin_Change_Resolution(void)
	{
	rprintfInit(uart3SendByte);//change UART to Blackfin
	rprintf("a");//Send Blackfin command for 160x128 resolution

	Blackfin_Echo(2);
	}


//Get version info, wait, change resolution, wait
void Blackfin_Initialize(void)
	{
	rprintfInit(uart3SendByte);//change UART to Blackfin
	//rprintf("vz");//reset blackfin
	//Blackfin_Echo(60);//collect version info
	//delay_cycles(1000);
	//must change resolution or it breaks!!!
	Blackfin_Change_Resolution();

	//must delay so that auto-brightness has time to adjust
	delay_ms(2000);
	}


//Find Mean Color
//returns   ##vmean  90 128 127
//          0123456789012345678
/*
I believe 'vm'
command response header is 8 characters - "##vmean ", and then the Y,
U and V are each 4 characters, followed by the \n

Luminence, Chroma Blue, Chroma Red
*/
void Blackfin_YUV_Mean(void)
	{
	//int temp10;

	rprintfInit(uart3SendByte);//change UART to Blackfin
	uartFlushReceiveBuffer(3);//flush out receive camera buffer to stop phase shifting

	rprintf("vm");//Send Blackfin command to find mean

	Blackfin_Echo(19);

	//output array
	/*
	for(temp10=0;temp10<19;temp10++)
		rprintf("%d ",response[temp10]);
	rprintf("\r\n");*/

	//decode array by summing up each part
	Y_mean=response[8]*100  + response[9]*10  + response[10];
	U_mean=response[12]*100 + response[13]*10 + response[14];
	V_mean=response[16]*100 + response[17]*10 + response[18];

	//stay within limits, causes send command error if its out of bounds
	/*if (Y_mean < 0)
		Y_mean=0;
	if (U_mean < 0)
		U_mean=0;
	if (V_mean < 0)
		V_mean=0;
	if (Y_mean > 255)
		Y_mean=255;
	if (U_mean > 255)
		U_mean=255;
	if (V_mean > 255)
		V_mean=255;*/

	//output for testing
	rprintfInit(uart0SendByte);//change UART to bluetooth
	rprintf("Y=%d U=%d V=%d\r\n\r\n",Y_mean, U_mean, V_mean);
	}


//converts YUV mean values to use for RGB mean values
//got equation from here: http://www.fourcc.org/fccyvrgb.php
//need to write a version that doesn't use floating point!!!
void Blackfin_YUV_2_RGB_accurate(void)
	{
	rprintfInit(uart0SendByte);//change UART to USB

	R_mean = 1.164*(Y_mean - 16) + 1.596*(V_mean - 128);

	G_mean = 1.164*(Y_mean - 16) - 0.813*(V_mean - 128) - 0.391*(U_mean - 128);

	B_mean = 1.164*(Y_mean - 16) + 2.018*(U_mean - 128);

	rprintf("R=%d G=%d B=%d\r\n\r\n",R_mean, G_mean, B_mean);
	}



//sets the range of a color bin (0-9)
//example:	vc0090255000255000255      returns ##vc0
//          012345678901234567890
//sets color bin #0 using Y mean, 255, 0, 255, 0, 255 so
//pixels of higher than average brightness will appear in blob search
void Blackfin_Bin_Range(int bin, int y_min, int y_max, int u_min, int u_max, int v_min, int v_max)
	{
	rprintfInit(uart3SendByte);//change UART to Blackfin
	uartFlushReceiveBuffer(3);//flush out receive camera buffer to stop phase shifting

	rprintf("vc%d",bin);//start command, send bin #
	rprintf("%d%d%d",y_min/100,y_min%100/10,y_min%100%10);
	rprintf("%d%d%d",y_max/100,y_max%100/10,y_max%100%10);
	rprintf("%d%d%d",u_min/100,u_min%100/10,u_min%100%10);
	rprintf("%d%d%d",u_max/100,u_max%100/10,u_max%100%10);
	rprintf("%d%d%d",v_min/100,v_min%100/10,v_min%100%10);
	rprintf("%d%d%d",v_max/100,v_max%100/10,v_max%100%10);

	Blackfin_Echo(6);//maybe 5?

	
	//test code
	rprintfInit(uart0SendByte);//change UART to bluetooth

	rprintf("vc%d",bin);//start command, send bin #
	rprintf("%d%d%d",y_min/100,y_min%100/10,y_min%100%10);
	rprintf("%d%d%d",y_max/100,y_max%100/10,y_max%100%10);
	rprintf("%d%d%d",u_min/100,u_min%100/10,u_min%100%10);
	rprintf("%d%d%d",u_max/100,u_max%100/10,u_max%100%10);
	rprintf("%d%d%d",v_min/100,v_min%100/10,v_min%100%10);
	rprintf("%d%d%d\r\n",v_max/100,v_max%100/10,v_max%100%10);
	}


//vb - searches for blobs matching the range of colors in a color bin
/*
vb0
returns  ##vb0  24  78  24  63    1123    (as an example)
		 01234567890123456789012345678
on a coordinate system of 80x64, a blob above of 1123 (pixel count)
triggering pixels was found in
              the box bounded by x1, x2, y1, y2 = 24, 78, 24, 63

Note:This function can return multiple blobs if they exist!
*/
//to track green, use low U and low V
//to track blue and green, use low V only
//to see color space:http://en.wikipedia.org/wiki/YUV
void Blackfin_Blob_Detect(int bin_value)
	{
	rprintfInit(uart3SendByte);//change UART to Blackfin
	uartFlushReceiveBuffer(3);//flush out receive camera buffer to stop phase shifting

	rprintf("vb%d",bin_value);//poll for blob of a particular bin

	Blackfin_Echo(99);
	
	//decode array by summing up each part, Y_top, Y_bottom, X_left, X_right;
	X_left=response[6]*100	  + response[7]*10  + response[8];
	X_right=response[10]*100  + response[11]*10 + response[12];
	Y_top=response[14]*100	  + response[15]*10 + response[16];
	Y_bottom=response[18]*100 + response[19]*10 + response[20];
	//long ints blow up in rprintf so divide by 10
	pixels=(response[25]*1000  + response[26]*100 + response[27]*10 + response[28]);

	X_centroid=(X_left+X_right)/2;
	Y_centroid=(Y_top+Y_bottom)/2;

	//test code, output centroid	
	rprintfInit(uart0SendByte);//change UART to bluetooth
	rprintf("X_left=%d, X_right=%d Y_top=%d Y_bottom=%d Pix=%d",X_left,X_right,Y_top,Y_bottom,pixels);
	rprintf(", X=%d Y=%d\r\n\r\n",X_centroid,Y_centroid);

	}


//rotate robot wrt camera location
//speed determines forward motion - set to 0 for only rotation
void ERP_body_track(int speed)
	{
	
	wheel_left(980-X_old*8-speed);
	wheel_right(980-X_old*8+speed);

	//straight
	//wheel_left(400);
	//wheel_right(1100);
	}


//moves camera to center on target: low numbers are top left, high are bottom right
//accepts: confidence (min # of pixels to track)
//higher damping ratio numbers reduce cam oscillation, but also slows it down
void Blackfin_HeadTrack(int confidence, int damping_ratio)
	{
	//large numbers allows cam to rotate further, but increases oscillation
	int x_multiplier=19;
	int y_multiplier=23;

	//horizontal 730 is middle
	//vertical 990 straight
	//40 is 1/2 of x resolution
	//32 is 1/2 of y resolution

	//track location of camera for robot body rotation, accounts for camera range
	cam_X_location=800 - (40*x_multiplier) + X_old*x_multiplier;
	cam_Y_location=990 - (32*y_multiplier) + Y_old*y_multiplier;

	//pixel check: ignores noise when object not around
	//checks if camera data is good: low lighting causes no vb to be returned!
	if(pixels>confidence && X_right < 81 && Y_bottom < 65)
		{
		//rotate camera
		camera_hor(cam_X_location);
		camera_ver(cam_Y_location);//active tracking
		//camera_ver(760);//fixed location for mobot

		//ERP_body_track(120);//120 is acceptable speed

		//store and average previous readings to slow camera down
		X_old=(X_centroid+X_old*(damping_ratio-1))/damping_ratio;
		Y_old=(Y_centroid+Y_old*(damping_ratio-1))/damping_ratio;
		}
	}




/*

vc - sets the range of a color bin (0-9)
vb - searches for blobs matching the range of colors in a color bin


vc0090255000255000255      returns ##vc0
              sets color bin #0 using Y mean, 255, 0, 255, 0, 255 so
pixels of higher than
              average brightness will appear in blob search

vb0        returns  ##vb0  24 78 24 63   1123    (as an example)
              on a coordinate system of 80x64, a blob above of 1123
triggering pixels was found in
              the box bounded by x1, x2, y1, y2 = 24, 78, 24, 63

You can sample at higher resolutions, but the 80x64 coordinate system
is a good starting point


------------------------

Just look for a '\n'  (0x0A) character.  These 'v' command responses
always start with "##" and end with \n

Also, the formating of the response strings is fixed, so the numbers
are always in the same place.  I can dig up the character positions
if you don't want to count.


------------------------


The vb response is  4 characters - "##vb", followed by 4 characters
each for x1, x2, y1, y2, and 8 characters for the pixel count for
each blob (the pattern repeats until \n)



-------------------------

'a' changes capture resolution to 160x128, with #a as the ack
'b' changes to 320x256, with #b as the ack
'I' dumps a JPEG frame

The command set is shown at http://www.surveyor.com/blackfin/SRV_blackfin_bf.html

A couple of things to try -

vc0000255000255000255
should return ##vc0

then

vhist0
should dump a histogram (in YUV)

You can try different settings for vc0 to narrow the color range and then send

vb0
which will give you a blob listing.


-------------------------
Whoops, I meant you need to strip off the 0x30 by masking with 0x0F, i.e.

Y_mean=(int)(response[8] & 0x0F)*100  + (int)(response[9]& 0x0F)*10  + (int)(response[10]&0x0F);

*/

/*
The gain control (auto-brightness) is pretty aggressive on this camera and I need to figure out how to manage it.  Also, I've been meaning to experiment with "shutter control" (exposure time).  Maybe I'll start to look at that next week.

I wish you could get the java console going.  I tested it here before uploading, so I don't understand what the problem would be.  I'm having my own set of problems with the Python console.  It is http://www.surveyor.com/tmp/SRV1Console-38k.zip that you installed, right ?  What version of  java are you running ?

You could try the python console.  Here's the syntax -
         python pySRV1Console.py -com COM4

You'll need to modify pySRV1Console.py to change the baud rate from 115200 to 38400. Details for download and configuration are here -
        http://www.surveyor.com/blackfin/#blackfin6

Not sure what to suggest on lens selection - I don't remember what I sent.   If there were 3 lenses (short one on camera plus 2 extras), the default lens has approx 60-deg field of view, and the other two are 90-deg and 120-deg field of view.
*/
