#ifndef MOTOR_SHB_H_
#define MOTOR_SHB_H_

#include <stdint.h>
#include "timer.h"

struct pin {
	uint8_t volatile *port;
	uint8_t mask;
};

struct pwm16_out {
	uint16_t volatile *mreg; /* the match compare register */
	struct pin p;
};

struct mshb {
	struct pwm16_out pwma;
	struct pwm16_out pwmb;
	struct pin enable;
};

#define PIN_INITIALIZER(port, idx) { .port = &(port), .mask = (1 << (idx)) }
#define PWM_INITIALIZER(reg, pin) { .mreg = (reg), .p = (pin) }
#define PWM_INITIALIZER2(reg, port, idx)			\
	PWM_INITIALIZER(reg, PIN_INITIALIZER(port, idx))
#define MSHB_INITIALIZER(pa, pb, en)				\
		{ .pwma = (pa), .pwmb = (pb), .enable = en }
#define MSHB_INITIALIZER2(rega, pina, regb, pinb, pinen)	\
		MSHB_INITIALIZER(PWM_INITIALIZER(rega, pina),	\
				PWM_INITIALIZER(regb, pinb),	\
				pinen)

#define PIN_INIT_OUT(pin) do {			\
	*((pin).port - 1) |= (pin).mask;	\
} while(0)

#define PIN_SET_HIGH(pin) do {			\
	*((pin).port) |= (pin).mask;		\
} while(0)

#define PIN_SET_LOW(pin) do {			\
	*((pin).port) |= ~(pin).mask;		\
} while(0)

#define PWM16_INIT(pwm16) do {			\
	PIN_INIT_OUT((pwm16).p);		\
	*((pwm16).mreg) = 0;			\
} while(0)

#define MSHB_INIT(mshb) do {			\
	PWM16_INIT((mshb).pwma);		\
	PWM16_INIT((mshb).pwmb);		\
	PIN_INIT_OUT((mshb).enable);		\
	PIN_SET_LOW((mshb).enable);		\
} while(0)

#define PB_2 PIN_INITIALIZER(PORTB, 2)
#define PB_1 PIN_INITIALIZER(PORTB, 1)
#define PD_7 PIN_INITIALIZER(PORTD, 7)
struct mshb mshb [] = {
	MSHB_INITIALIZER2(OC1B, PB_2, OC1A, PB_1, PD_7)
};

#define MSHB_CT (sizeof(ms) / sizeof(*ms))

static inline void mshb_init(void)
{
	/* Pin mappings:
	 * Digital 10 / OC1B / PB2 => PA / PWMA / IN (A)
	 * Digital  9 / OC1A / PB1 => PB / PWMB / IN (B)
	 * Digital  7 / PD7 => ENA / ENB / INH (A) / INH (B)
	 */

	TIMER1_PWM_INIT(INT16_MAX);
	uint8_t i;
	for (i = 0; i < MSHB_CT; i++) {
		MSHB_INIT(mshb[i]);
	}
}

static inline void mshb_enable(uint8_t idx)
{
	PIN_SET_HIGH(mshb[i].enable);
}

static inline void mshb_set(uint8_t idx, int16_t speed)
{
	 
}

#endif
