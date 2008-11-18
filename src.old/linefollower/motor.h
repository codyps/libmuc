/*	Motor subsystems	*/

#ifndef _MOTOR_H_
#define _MOTOR_H_ "motor.h"

#include "defines.h"
#include <avr/io.h>
#include <inttypes.h>

void set_motor_L(uint16_t);
void set_motor_R(uint16_t);
uint16_t get_motor_L(void);
uint16_t get_motor_R(void);
uint8_t	motor_mode_L(uint8_t);
uint8_t	motor_mode_R(uint8_t);

enum { MOTOR_MODE_GET, MOTOR_MODE_CW, MOTOR_MODE_CCW, MOTOR_MODE_STOP, MOTOR_MODE_SB };



#endif
