/*	Motor subsystems	*/

#ifndef _MOTOR_H_
#define _MOTOR_H_ "motor.h"

#include "defines.h"
#include <avr/io.h>
#include <inttypes.h>

// A / B defines (specified)
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

// Left / Right defines (calculated)
#define MOTOR_LEFT	M_PWMA
#define MOTOR_RIGHT	M_PWMB

#define M_LIN1 M_AIN1
#define M_LIN2 M_AIN2
#define M_RIN1 M_BIN1
#define M_RIN2 M_BIN2

#define MOTOR_L_FWD MOTOR_MODE_CW
#define MOTOR_L_BWD MOTOR_MODE_CCW
#define MOTOR_R_FWD MOTOR_MODE_CW
#define MOTOR_R_BWD MOTOR_MODE_CCW

// Modes
typedef enum { MOTOR_MODE_GET, MOTOR_MODE_CW, MOTOR_MODE_CCW, MOTOR_MODE_STOP,\
	MOTOR_MODE_SB, MOTOR_MODE_BWD, MOTOR_MODE_FWD, MOTOR_MODE_ERROR } motor_mode_t;

//    0		1     2
enum {LEFT, RIGHT,FWD};
enum {NEG,	POS};
#define SIGN(__A) (__A>=0)

void 		motor_set_speed	(uint16_t speed,uint8_t motor);
uint16_t	motor_get_speed	(uint8_t motor); 
uint8_t		motor_mode	(motor_mode_t mode, uint8_t motor);

void lf_turn_inc(uint16_t incriment,int8_t dir);
void lf_speed_full(void);
void lf_speed_stop(void);
void motors_init(void);
#endif
