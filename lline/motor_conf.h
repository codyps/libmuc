/*
PB5 (OC1D) -> PWMA
PA1 -> AIN1
PA2 -> AIN2

PB3 (OC1B) -> PWMB
PB6 -> BIN1
PB4 -> BIN2

M1/A -> right
M2/B -> left
*/
#include <math.h>
#include <stdint.h>

#define MOTOR_T1_PRESCALE 1
#define T1_PRESCALE_BITS(prescale)	\
	((uint8_t)(  0x0F & (uint8_t)(( log(prescale) + log(2) )/log(2)) ))


#define USE_OC1A 0
#define USE_OC1B 1 
#define USE_OC1D 1

#ifdef USE_OC1A_PWM
#define T1A_1 1
#define T1A_0 0
#define T1A_P 1
#endif

// value's sign indicates direction (attinyX61 has 10bit pwm)
typedef motor_speed_t int16_t;


