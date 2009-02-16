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

/******define servo functions******/
//define electronics/motors/servos to specific hardware pins

#define wheel_left( position )					servo(PORTE,6,position)
#define wheel_right( position )					servo(PORTC,7,position)

/**********************************/


/***********define UART************/
/*int bluetooth=0;
int USB=1;
int uart2=2;
int black_fin=3*/

/**********************************/


/*********define positions*********/
/*long int arm_left_base=600;
long int arm_left_J1=600;
long int arm_left_J2=600;

long int wheel_left=600;
long int wheel_right=600;

long int camera_hor=600;
long int camera_ver=600;*/
/**********************************/
