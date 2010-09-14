#ifndef MOTOR_CONF_H_
#define MOTOR_CONF_H_
#include <stdint.h>
#include <avr/io.h>
#include "motor_internal.h"


/* XXX: these are not real values. */
static const struct motor_s motors [] = {
	MOTOR_INIT(PORTB, 1, PORTB, 2, PORTB, 3, OCR1A),
	MOTOR_INIT(PORTB, 4, PORTB, 5, PORTB, 6, OCR1B),
};

#endif
