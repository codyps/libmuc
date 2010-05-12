#ifndef SERVO_CONF
#define SERVO_CONF

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
	.pos = CLICKS_MS(1) + CLICKS_MS(1)/2 }
static struct servo_s servo[] = {
	SERVO_INIT(PORTB, 2),
	SERVO_INIT(PORTB, 3),
};

/* max number of servos. determine by dividing
 * 20ms by TIMER_CYCLES. result must be > 2ms 
 */
#define TIMER_CYCLES 5 

#define SV_TIMER 1



#endif
