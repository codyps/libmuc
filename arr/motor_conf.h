#ifndef MOTOR_CONF_H_
#define MOTOR_CONF_H_
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

#define MOTOR_SPEED_MAX 0xFFFF

#define MOTOR_INIT(en_port, en_idx, dir_port, dir_idx,         \
		pwm_port, pwm_idx, pwm_reg)                    \
	{ &(en_port), 1<<(en_idx), &(dir_port), 1<<(dir_idx),  \
		&(pwm_port), 1<<(pwm_idx), &(pwm_reg) }

/* XXX: these are not real values. */
static const struct motor_s motors [] = {
	MOTOR_INIT(PORTB, 1, PORTB, 2, PORTB, 3, OCR1A),
	MOTOR_INIT(PORTB, 4, PORTB, 5, PORTB, 6, OCR1B),
};
#define MOTORS_CT (sizeof(motors) / sizeof(*motors))

#endif
