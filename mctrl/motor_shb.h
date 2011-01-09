#ifndef MOTOR_SHB_H_
#define MOTOR_SHB_H_

#include <stdint.h>
#include "timer.h"
#include "common.h"

struct pin {
	uint8_t volatile *port;
	uint8_t mask;
};

struct pwm16_out {
	uint16_t volatile *mreg; /* the match compare register */
	struct pin p;
};

struct pwm8_out {
	uint8_t volatile *mreg;
	struct pin p;
};

struct mshb {
	/* the atmega328p only has 2 16bit pwms, so we sacrafice pwm
	 * accuracy in the reverse */
	struct pwm16_out pwma;
	struct pwm8_out pwmb;
	struct pin enable;
};

#define PIN_INITIALIZER(port_, idx) { .port = &(port_), .mask = (1 << (idx)) }
#define PWM_INITIALIZER(reg, pin) { .mreg = &(reg), .p = pin }
#define MSHB_INITIALIZER(pa, pb, en)				\
		{ .pwma = pa, .pwmb = pb, .enable = en }

#define PB_1 PIN_INITIALIZER(PORTB, 1)
#define PB_2 PIN_INITIALIZER(PORTB, 2)
#define PD_5 PIN_INITIALIZER(PORTD, 5)
#define PD_6 PIN_INITIALIZER(PORTD, 6)
#define PD_7 PIN_INITIALIZER(PORTD, 7)

#define TMR0_PWMA PWM_INITIALIZER(OCR0A, PD_6)
#define TMR0_PWMB PWM_INITIALIZER(OCR0B, PD_5)
#define TMR1_PWMA PWM_INITIALIZER(OCR1A, PB_1)
#define TMR1_PWMB PWM_INITIALIZER(OCR1B, PB_2)

struct mshb mshb_d [] = {
	MSHB_INITIALIZER(TMR1_PWMA, TMR0_PWMA, PD_7),
	MSHB_INITIALIZER(TMR1_PWMB, TMR0_PWMB, PD_7)
};

#define PIN_INIT_OUT(pin) do {					\
	*((pin).port - 1) |= (pin).mask;			\
} while(0)

#define PIN_SET_HIGH(pin) do {					\
	*((pin).port) |= (pin).mask;				\
} while(0)

#define PIN_SET_LOW(pin) do {					\
	*((pin).port) |= (uint8_t)~(pin).mask;			\
} while(0)

#define PWM_INIT(pwm) do {					\
	PIN_INIT_OUT((pwm).p);					\
	*((pwm).mreg) = 0;					\
} while(0)

#define MSHB_INIT(mshb) do {					\
	PWM_INIT((mshb).pwma);					\
	PWM_INIT((mshb).pwmb);					\
	PIN_INIT_OUT((mshb).enable);				\
	PIN_SET_LOW((mshb).enable);				\
} while(0)

#define PWM16_SET(pwm, val15) do {				\
	*((pwm).mreg) = (uint16_t)(val15);			\
} while (0)

#define PWM8_SET(pwm, val15) do {				\
	*((pwm).mreg) = (uint8_t)((uint16_t)(val15) >> (15-8));	\
} while (0)

static inline void mshb_init(void)
{
	/* Pin mappings:
	 * Digital 10 / OC1B / PB2 => PA / PWMA / IN (A)
	 * Digital  9 / OC1A / PB1 => PB / PWMB / IN (B)
	 * Digital  7 / PD7 => ENA / ENB / INH (A) / INH (B)
	 */
	TIMER0_INIT_PWM_MAX();
	TIMER1_INIT_PWM(INT16_MAX);
	uint8_t i;
	for (i = 0; i < ARRAY_SIZE(mshb_d); i++) {
		MSHB_INIT(mshb_d[i]);
	}
}

static inline void mshb_enable(uint8_t i)
{
	PIN_SET_HIGH(mshb_d[i].enable);
}

static inline void mshb_disable(uint8_t i)
{
	PIN_SET_LOW(mshb_d[i].enable);
}

static inline void mshb_set(uint8_t i, int16_t speed)
{
	if (speed < 0) {
		PWM16_SET(mshb_d[i].pwma, 0);
		PWM8_SET(mshb_d[i].pwmb, -speed);
	} else {
		PWM8_SET(mshb_d[i].pwmb, 0);
		PWM16_SET(mshb_d[i].pwma, speed);
	}
}

#endif
