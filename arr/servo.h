/*
	Servo Control Definitions
*/
#ifndef _SERVO_H_
#define _SERVO_H_

#include <stdint.h>
#include <avr/io.h>
/* Servo Interface. */

void servo_init(void);
int8_t servo_set(uint8_t servo_number, uint16_t servo_val);
uint16_t servo_get(uint8_t servo_number);
uint8_t servo_ct(void);

#define servo_set_us(index,us) servo_set(index,CLICKS_US(us))

/* Defines */

#define TIMER_PRESCALE_1 ( (0<<CS2) | (0<<CS1) | (1<<CS0) )
#define TIMER_PRESCALE_8 ( (0<<CS2) | (1<<CS1) | (0<<CS0) )

#endif // _SERVO_H_

/** Data for Axon w/ RoboMagellan HW **/


/*
aproximatly 2400 us and 1600 us give 90 degree angles (actualy slightly less)
*/

/* Math */

// clicks = F_CPU / hz
// ms   | secs   | hz                   |  clicks@16e6Hz
// 20	= 2e-2  => 50			=> 320000
// 18	= 18e-3	=>
// 0.5	= 5e-4	=> 2000			=> 8000
// 1	= 1e-3  => 1000			=> 16000
// 1.5	= 15e-4	=> 666 + 2/3	=> 24000
// 2	= 2e-3  => 500			=> 32000
// 2.5	= 25e-4	=> 400			=> 40000
// 3	= 3e-3  => 333+1/3		=> 48048.048...
// 4    = 4e-3  => 250			=> 64000 = 0xfa00

// max = F_CPU/(1/.0002)-F_CPU/(1/.0001)


/* Timer Documentation */

// PWM A L = H3 = OC4A
// PWM A R = H4 = OC4B
// PWM B L = C3 = NONE
// PWM B R = C1 = NONE

// SERVO P = A4 = NONE
// SERVO T = A5 = NONE
// SERVO IRL = A6/A7 = NA
// SERVO IRR = A7/J6 = NA


/* All Timers
 0,2 = 8bit
 1,3,4,5 = 16-bit

 1:(A,B,C)
	B7 = OC0A/OC1C
	B6 = OC1B
	B5 = OC1A
 3:(A,B,C)
eN	E5 = OC3C
eN	E4 = OC3B
	E3 = OC3A
 4:(A,B,C)
	H5 = OC4C
mU	H4 = OC4B
mU	H3 = OC4A
 5:(A,B,C)
X	L5 = OC5C
X	L4 = OC5B
X	L3 = OC5A

 B7 = OC0A/OC1C
 G5 = OC0B
 B4 = OC2A
 H6 = OC2B
*/
