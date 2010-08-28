#ifndef MOTOR_H_
#define MOTOR_H_
#include <stdint.h>

#define MOTOR_SPEED_MAX INT16_MAX
void motors_init(void);
void motor_set(uint8_t idx, int16_t speed);

#endif /* MOTOR_H_ */
