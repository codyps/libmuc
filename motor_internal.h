#ifndef MOTOR_INTERNEL_H_
#define MOTOR_INTERNAL_H_ 1

#include <stdint.h>
#include <avr/io.h>

struct motor_s {
	volatile uint8_t *enable_port;
	uint8_t enable_mask;

	volatile uint8_t *dir_port;
	uint8_t dir_mask;

	volatile uint8_t *pwm_port;
	uint8_t pwm_mask;

#if defined(__AVR_ATmega328P__)
	volatile uint16_t *pwm_reg;
#else
# error "Motor reg type funny"
#endif
};

#define MOTOR_SPEED_MAX INT16_MAX
#define MOTOR_INIT(en_port, en_idx, dir_port, dir_idx,         \
		pwm_port, pwm_idx, pwm_reg)                    \
	{ &(en_port), 1<<(en_idx), &(dir_port), 1<<(dir_idx),  \
		&(pwm_port), 1<<(pwm_idx), &(pwm_reg) }

#define MOTORS_CT (sizeof(motors) / sizeof(*motors))
#endif
