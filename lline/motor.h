#ifndef _MOTOR_H_
#define _MOTOR_H_
#include "motor_conf.h"

void motor_init(void);

void			motor_set(uint8_t motor_index, motor_speed_t value);
motor_speed_t	motor_get(uint8_t motor_index);

#endif
