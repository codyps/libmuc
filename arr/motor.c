
#include <stdint.h>
#include "motor.h"
#include "motor_conf.h"

/* 15bits.
 * Makes handling rounding conditions much simpler */
#define TIMER1_TOP INT16_MAX

static void pwm_timer1_init(void)
{
	/* Table 15-4. Waveform Generation Mode Bit Description
	 * n  WGM13:0 Desc                        TOP    OCR1x_up. TOV1_flag_set
	 * 0  0 0 0 0 Normal                      0xFFFF Immediate MAX
	 * 1  0 0 0 1 PWM, Phase Correct, 8-bit   0x00FF TOP       BOTTOM
	 * 2  0 0 1 0 PWM, Phase Correct, 9-bit   0x01FF TOP       BOTTOM
	 * 3  0 0 1 1 PWM, Phase Correct, 10-bit  0x03FF TOP       BOTTOM
	 * 4  0 1 0 0 CTC                         OCR1A  Immediate MAX
	 * 5  0 1 0 1 Fast PWM, 8-bit             0x00FF BOTTOM    TOP
	 * 6  0 1 1 0 Fast PWM, 9-bit             0x01FF BOTTOM    TOP
	 * 7  0 1 1 1 Fast PWM, 10-bit            0x03FF BOTTOM    TOP
	 * 8  1 0 0 0 PWM, Phase and Freq Correct ICR1   BOTTOM    BOTTOM
	 * 9  1 0 0 1 PWM, Phase and Freq Correct OCR1A  BOTTOM    BOTTOM
	 * 10 1 0 1 0 PWM, Phase Correct          ICR1   TOP       BOTTOM
	 * 11 1 0 1 1 PWM, Phase Correct          OCR1A  TOP       BOTTOM
	 * 12 1 1 0 0 CTC                         ICR1   Immediate MAX
	 * 13 1 1 0 1 (Reserved)                  –      –         –
	 * 14 1 1 1 0 Fast PWM                    ICR1   BOTTOM    TOP
	 * 15 1 1 1 1 Fast PWM                    OCR1A  BOTTOM    TOP
	 */

	/* Stop timer and set part of waveform generation 
	 * WGM13:2 = 10 */
	TCCR1B = (1 << WGM13) | (0 << WGM12);

	/* Enable bother compare match outputs. (COM1XY) */
	TCCR1A = (1 << COM1A1) | (1 << COM1A0)
		|(1 << COM1B1) | (1 << COM1B0)
		|(0 << WGM11 ) | (0 << WGM10 );

	TCNT1 = 0;
	OCR1A = 0;
	OCR1B = 0;
	ICR1 = TIMER1_TOP; /* TOP */

	/* no interrupts for us */
	TIMSK1 = (0 << ICIE1)
		|(0 << OCIE1B)
		|(0 << OCIE1A)
		|(0 << TOIE1);

	/* Clear all the interrupt flags */
	TIFR1 = (1 << ICF1)
		| (1 << OCF1B)
		| (1 << OCF1A)
		| (1 << TOV1);

	/* Set waveform (again) and enable (prescaler bits != 0) */
	TCCR1B = (0 << ICNC1 ) | (0 << ICES1 )
		|(1 << WGM13 ) | (0 << WGM12 )
		|(0 << CS12) | (0 << CS11) | (1 << CS10);
}

void motors_init(void)
{
	uint8_t i;
	pwm_timer1_init();
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
