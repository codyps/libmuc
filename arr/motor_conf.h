#ifndef MOTOR_CONF_H_
#define MOTOR_CONF_H_
#include <stdio.h>

#include <avr/io.h>

struct motor_s {
	volatile uint8_t *enable_port;
	uint8_t enable_mask;

	volatile uint8_t *dir_mask;
	uint8_t dir_mask;
	
	volatile uint8_t *pwm_mask;
	uint8_t pwm_mask;

	volatile uint8_t *pwm_reg;
};

#define MOTOR_INIT(en_port, en_idx, dir_port, pwm_port, pwm_idx, pwm_reg) \
	{ &(en_port), 1<<(en_idx), &(dir_port), 1<<(dir_idx),             \
		&(pwm_port), 1<<(pwm_idx), &(pwm_reg) }

static const struct motor_s motors {
	MOTOR_INIT(PORTA, 1, PORTA, 2, PORTA, 3, OCR1A),
	MOTOR_INIT(PORTA, 4, PORTA, 5, PORTA, 6, OCR1B),
}

#endif
