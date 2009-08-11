#include <avr/io.h>

#include <math.h>
#include <stdint.h>

/* Configuration Helpers/ETC */
// Prescale Calculation
#define T1_PRESCALE_BITS(prescale)	\
	((uint8_t)(  0x0F & (uint8_t)(( log(prescale) + log(2) )/log(2)) ))

// Types
#define motor_speed_t int16_t
struct motor_pwm {
	volatile uint8_t * reg_pwm;
	volatile uint8_t * reg_pwmh;
	volatile uint8_t * port_p1;
	volatile uint8_t * port_p2;
	uint8_t mask_p1;
	uint8_t mask_p2;
};

// Motor structure constructor
#define MOTOR_DEF(reg, port1, index1, port2, index2) \
	{ \
		.reg_pwm = &reg, \
		.reg_pwmh= &TC1H, \
		.port_p1 = &port1, \
		.port_p2 = &port2, \
		.mask_p1 = (uint8_t) ( 1 << (index1) ), \
		.mask_p2 = (uint8_t) ( 1 << (index2) ), \
	}

/* Configuable Items */
// Motor listing
const static struct motor_pwm motor_list [] = {
	MOTOR_DEF(OCR1D,PORTA,1,PORTA,2),
	MOTOR_DEF(OCR1B,PORTB,6,PORTB,4)
};
#define MOTOR_CT ( sizeof(motor_list) )

// Register usage
#define USE_OC1A 0
#define USE_OC1B 1
#define USE_OC1D 1

// Prescale selection
#define MOTOR_T1_PRESCALE 1

/* Physical Data (lline erector)
PB5 (OC1D) -> PWMA
PA1 -> AIN1
PA2 -> AIN2

PB3 (OC1B) -> PWMB
PB6 -> BIN1
PB4 -> BIN2

M1/A -> right
M2/B -> left
*/
