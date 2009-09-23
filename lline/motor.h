#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>

// Types
#define MOTOR_SPEED_MAX 0x3FF
typedef int16_t  motor_speed_t;  //only 11 bits used, 10+sign
typedef uint16_t motor_uspeed_t;

struct pin_s {
  volatile uint8_t * const port;
  const uint8_t mask;
};

typedef struct motor_s {
	volatile uint8_t * const reg_pwm;
	volatile uint8_t * const reg_pwmh;
	const struct pin_s pin[2];
} motor_t;

// Motor structure constructor
#define MOTOR_DEF(reg, port1, index1, port2, index2) \
	{ \
		.reg_pwm = &reg, \
		.reg_pwmh= &TC1H, \
		.pin[0].port = &port1, \
		.pin[1].port = &port2, \
		.pin[0].mask = (uint8_t) ( 1 << (index1) ), \
		.pin[1].mask = (uint8_t) ( 1 << (index2) ) \
}

// Functions
void motor_init(void);

void motor_set(motor_t *motor, motor_speed_t value);
motor_speed_t motor_get(motor_t *motor);

#endif
