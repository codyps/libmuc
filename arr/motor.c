
#include "motor.h"
#include "motor_conf.h"

void motors_init(void)
{
	uint8_t i;
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

static int16_t scale_speed(const struct motor_s *motor, int16_t speed)
{
	return (uint32_t)(speed * MOTOR_SPEED_MAX) / INT16_MAX;
}

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


	uint8_t sval = scale_speed(motor, speed);

	/* XXX: driver specific setting */
	*(motor->pwm_reg) = sval;
}
