/* 
	Servo Control Definitions 
*/

// F_CPU/(Prescaler*(1+(TOP)) = 50
// 16e6/(8*(1+(40000-1)) = 50
#define TIMER5_TOP (40000-1)

// Prescaler = 8 => 010 
//(0<<CS2)|(1<<CS1)|(0<<CS0);
#define TIMER5_PRESCALE ((0<<CS2)|(1<<CS1)|(0<<CS0))


// Servo Definitions
#define SERVO_P   0
#define SERVO_T	  1
#define SERVO_IRL 2
#define SERVO_IRR 3

#define SERVO_P_INDEX   4
#define SERVO_T_INDEX	5
#define SERVO_IRL_INDEX 6
#define SERVO_IRR_INDEX 7

#define SERVO_P_PORT   PORTA
#define SERVO_T_PORT   PORTA
#define SERVO_IRL_PORT PORTA
#define SERVO_IRR_PORT PORTA


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




