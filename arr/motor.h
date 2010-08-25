#ifndef MOTOR_H_
#define MOTOR_H_
#include <stdint.h>

void motors_init(void);
void motor_set(uint8_t idx, int16_t speed);

#endif /* MOTOR_H_ */
