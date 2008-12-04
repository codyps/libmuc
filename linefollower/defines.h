/*
Globaly Utilized Information
*/


#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define F_CPU 8000000

/* Amount Defines for Line Following */
#define LF_ADC_MIX_WIEGHT 5	//Multiplied by the 2 middle adc values to wieght them
#define LF_INC 0x200	// Turn Incriment
#define LF_MAX_SPEED 0x6000	// Software implimentation sucks. anything close to the 0 or ffff
#define LF_MIN_SPEED 0x1500 // has undefined behavior.
/* A / B defines */

#define M_PWMA OCR1A
#define M_PWMB OCR1B

#define M_PWMA_PIN 2
#define M_PWMB_PIN 3

#define MOTOR_PWM_PORT PORTB
#define MOTOR_PWM_DDR DDRB
#define MOTOR_CTL_PORT PORTD
#define MOTOR_CTL_DDR DDRD

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

/* Mode/State */
enum {WAIT,TEST,FOLLOW};
uint8_t c_mode;
bool initial;

#endif
