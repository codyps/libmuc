#include <avr/io.h>
#include <stdint.h>
#include "motor.h"
#include "motor_conf.h"
#include "timer.h"

/* 15bits.
 * Makes handling rounding conditions much simpler */
#define TIMER1_TOP INT16_MAX

void motors_init(void)
{
	uint8_t i;
	TIMER1_INIT_PWM(TIMER1_TOP);
	for(i = 0; i < MOTORS_CT; i++) {
		const struct motor_s *motor = motors + i;
		*(motor->dir_port - 1) |= motor->dir_mask;

		*(motor->enable_port - 1) |= motor->enable_mask;
		*(motor->enable_port) |= motor->enable_mask;

		*(motor->pwm_port - 1) |= motor->pwm_mask;
		*(motor->pwm_reg) = 0;
	}
}

static void motor_set_forward(const struct motor_s *motor)
{
	*(motor->dir_port) |= motor->dir_mask;
}

static void motor_set_reverse(const struct motor_s *motor)
{
	*(motor->dir_port) &= ~(motor->dir_mask);
}

#if (MOTOR_SPEED_MAX != TIMER1_TOP)
static uint16_t scale_speed(const struct motor_s *motor, uint16_t speed)
{
	return (uint32_t)(speed * MOTOR_SPEED_MAX) / TIMER1_TOP;
}
#endif

void motor_set(uint8_t idx, int16_t speed)
{
	const struct motor_s *motor = motors + idx;

	if (speed >= 0) {
		motor_set_forward(motor);
	} else {
		motor_set_reverse(motor);

		/* XXX: this transform is driver specific */
		speed = -speed;
	}
#if (MOTOR_SPEED_MAX != TIMER1_TOP)
	uint16_t sval = scale_speed(motor, speed);
	*(motor->pwm_reg) = sval;
#else
	/* XXX: driver specific setting */
	*(motor->pwm_reg) = speed;
#endif
}
