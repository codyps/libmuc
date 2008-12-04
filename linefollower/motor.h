/*	Motor subsystems	*/

#ifndef _MOTOR_H_
#define _MOTOR_H_ "motor.h"

#include "defines.h"
#include <avr/io.h>
#include <inttypes.h>

void 		motor_set_speed	(uint16_t speed,uint8_t motor);
uint16_t	motor_get_speed	(uint8_t motor); 
uint8_t		motor_mode		(uint8_t mode, uint8_t motor);


enum { MOTOR_MODE_GET, MOTOR_MODE_CW, MOTOR_MODE_CCW, MOTOR_MODE_STOP, MOTOR_MODE_SB, MOTOR_MODE_BWD, MOTOR_MODE_FWD, MOTOR_MODE_ERROR };
enum {LEFT, RIGHT};

void lf_turn_inc(int32_t incriment);
void lf_turn_left_inc(uint16_t incriment)__attribute__((__deprecated__));
void lf_turn_right_inc(uint16_t incriment)__attribute__((__deprecated__));
void lf_full_speed(void);

#endif
