/*
Globaly Utilized Information
*/


#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <avr/io.h>
#define F_CPU 8000000

/* A / B defines */

#define M_PWMA OCR1A
#define M_PWMB OCR1B

#define MOTOR_PORT PORTD

#define M_AIN1 PD1
#define M_AIN2 PD3
#define M_BIN1 PD5
#define M_BIN2 PD7

/* Left / Right defines */

#define MOTOR_LEFT	OCR1A
#define MOTOR_RIGHT	OCR1B

#define M_LIN1 M_AIN1
#define M_LIN2 M_AIN2
#define M_RIN1 M_BIN1
#define M_RIN2 M_BIN2

#define MOTOR_L_FWD MOTOR_MODE_CW
#define MOTOR_L_BWD MOTOR_MODE_CCW
#define MOTOR_R_FWD MOTOR_MODE_CW
#define MOTOR_R_BWD MOTOR_MODE_CCW

#endif
