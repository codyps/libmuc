/*	Motor subsystems	*/

#ifndef _MOTOR_H_
#define _MOTOR_H_ "motor.h"

#include "defines.h"
#include <avr/io.h>
#include <inttypes.h>


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

void motor_turn_inc(uint16_t incriment,int8_t dir);
void motor_speed_full(void);
void motor_speed_stop(void);
void motors_init(void);
#endif
