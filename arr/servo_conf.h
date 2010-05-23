#ifndef SERVO_CONF_H_
#define SERVO_CONF_H_

#include "clock.h"

/* Servo Informaiton and State  */
struct servo_s {
	volatile uint16_t pos; // position
	volatile uint8_t *const port;
	const uint8_t mask;
};

#define SERVO_INIT(_port,_index) {           \
	.port = &(_port),                    \
	.mask = (uint8_t) 1<<(_index),       \
	.pos = TICKS_MS(1) + TICKS_MS(1)/2 }
static struct servo_s servo[] = {
	SERVO_INIT(PORTB, 1), // 9
	SERVO_INIT(PORTB, 2), // 10
	SERVO_INIT(PORTB, 3), // 11
};

/* max number of servos. determine by dividing
 * 20ms by SV_TIMER_CYCLES. result must be > 2ms
 */
#define SV_TIMER_CYCLES 8

#define SV_TIMER 1

#endif
