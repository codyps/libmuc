#include <avr/io.h>

#include <stdint.h>

/* Configuable Items */
// Motor listing
static const struct motor_s motor_list [] = {
	MOTOR_DEF(OCR1D,PORTA,1,PORTA,2),
	MOTOR_DEF(OCR1B,PORTB,6,PORTB,4)
};
#define MOTOR_CT ( sizeof(motor_list) / sizeof(motor_s) )

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
