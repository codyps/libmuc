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


// A / B defines (specified)
#define M_PWMA OCR1D
#define M_PWMB OCR1B

#define M_PWMA_PIN 5
#define M_PWMB_PIN 3

#define MOTOR_PWM_PORT PORTB
#define MOTOR_PWM_DDR DDRB
#define MOTOR_CTL_PORT PORTD
#define MOTOR_CTL_DDR DDRD

#define M_AIN1 PD1
#define M_AIN2 PD3
#define M_BIN1 PD5
#define M_BIN2 PD7

// Left / Right defines (calculated)
#define MOTOR_LEFT	M_PWMA
#define MOTOR_RIGHT	M_PWMB

#define M_LIN1 M_AIN1
#define M_LIN2 M_AIN2
#define M_RIN1 M_BIN1
#define M_RIN2 M_BIN2

#define MOTOR_L_FWD MOTOR_MODE_CW
#define MOTOR_L_BWD MOTOR_MODE_CCW
#define MOTOR_R_FWD MOTOR_MODE_CW
#define MOTOR_R_BWD MOTOR_MODE_CCW


