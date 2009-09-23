/*
	Low level motor control via timer based pwm.
*/
#include "motor.h"
#include "math.h"

#include <stdbool.h>
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

// Register usage
#define USE_OC1A 0
#define USE_OC1B 1
#define USE_OC1D 1

// Prescale selection
#define MOTOR_T1_PRESCALE 1

struct pwm_param_s {
  uint8_t use_oc1a:1;
  uint8_t use_oc1b:1;
  uint8_t use_oc1d:1;
  uint8_t t1_prescale:5;
} const pwm_param = { USE_OC1A, USE_OC1B, USE_OC1D, MOTOR_T1_PRESCALE };

static void motor_pwm10_init(struct pwm_param_s param);
static inline void motor_pins(motor_t *motor, uint8_t dir1, uint8_t dir2);
static inline void motor_pin(motor_t *motor, uint8_t pin, uint8_t dir);
static inline void motor_pin_h(motor_t *motor, uint8_t pin);
static inline void motor_pin_l(motor_t *motor, uint8_t pin);

void motor_init(void) {
	motor_pwm10_init(pwm_param);
}

#define T1_PRESCALE_BITS(prescale)	\
	((uint8_t)(  0x0F & (uint8_t)(( log(prescale) + log(2) )/log(2)) ))

static void motor_pwm10_init(const struct pwm_param_s param) {
	// Initialize a 10 bit pwm (as on the attinyX61 series)
	//   All done by the datasheet.
	
	power_timer1_enable();
	
	PLLCSR = (1<<PLLE);
	_delay_us(100);
	loop_until_bit_is_set(PLLCSR,PLOCK);
	PLLCSR |= (1<<PCKE);
	
	/*

	Compare Output Mode, Phase and Frequency Correct PWM Mode
	COM1x1..0  OCW1x Behaviour                         OC1x  !OC1x Pin
	00     Normal port operation.                        o     o
		        
	01     Cleared on Compare Match when up-counting.    x     x
		  Set on Compare Match when down-counting.

	10     Cleared on Compare Match when up-counting.    x     o
		  Set on Compare Match when down-counting.

	11     Set on Compare Match when up-counting.        x     o
		  Cleared on Compare Match when down-counting.
	(x=connected,o=disconnected)
	*/
	/*
		                                                 Update of TOV1 Flag
	PWM1x WGM11..10 Timer/Counter Mode of Operation TOP   OCR1x at  Set on
	  0     xx      Normal                          OCR1C Immediate TOP
	  1     00      Fast PWM                        OCR1C TOP       TOP
	  1     01      Phase and Frequency Correct PWM OCR1C BOTTOM    BOTTOM
	  1     10      PWM6 / Single-slope             OCR1C TOP       TOP
	  1     11      PWM6 / Dual-slope               OCR1C BOTTOM    BOTTOM

	*/

	TCCR1A = ((param.use_oc1a)<<COM1A1) | (0<<COM1A0) | // Output pin modes (OCR1A)
		((param.use_oc1b)<<COM1B1) | (0<<COM1B0) | // ^^               (OCR1B)
		(       0<<FOC1A ) | (0<<FOC1B ) | // Force output compare
		((param.use_oc1a)<<PWM1A ) |
		((param.use_oc1b)<<PWM1B ) ;

	/*
	TCCR1B = (0<<PWM1X ) | // PWM inversion (switch !OCR1x and OCR1x)
		(0<<PSR1  ) | // Prescaler reset, cleared by hw
		(0<<DTPS11) | (0<<DTPS10); // Dead time prescaler bits
		//CS1{3,2,1,0}: prescale bits
	*/
		
	TCCR1C = ((param.use_oc1a)<<COM1A1S)|(0<<COM1A0S) | // Shadow bits
		((param.use_oc1b))|(0<<COM1B0S) | // ^^
		((param.use_oc1d)<<COM1D1) | (0<<COM1D0) | // Output pin modes D
		(0<<FOC1D ) | // Force output compare
		((param.use_oc1d)<<PWM1D ) ;

	/*
	TCCR1D = (0<<FPIE1) | // Fault protection interrupt enable
		(0<<FPEN1) | // FP enable
		(0<<FPNC1) | // FP noise canceller
		(0<<FPES1) | // FP edge select
		(0<<FPAC1) | // FP analog comparitor enable
		(0<<FPF1 ) | // FP interrupt flag
		(0<<WGM11) | (1<<WGM10); // Waveform select
	*/
	TCCR1D |= (1<<WGM10);

	/*	
	TCCR1E = (0<<7) | (0<<6) | // Reserved (on attiny861)
		// Output compare override enable (PWM6). OC1OEx = PBx
		(0<<OC1OE5) | (0<<OC1OE4) | (0<<OC1OE3) | 
		(0<<OC1OE2) | (0<<OC1OE1) | (0<<OC1OE0) ;
	*/

	// Shared with T0
	//TIMSK &= (uint8_t) ~((0<<OCIE1D) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1));
	//TIFR  |= (1<<OCF1D ) | (1<<OCF1A ) | (1<<OCF1B ) | (1<<TOV1 );	

	//TC1H  = 0;
	//TCNT1 = 0;
	//OCR1A = 0;
	//OCR1B = 0;
	//OCR1D = 0;
	
	TC1H  = 0b11;
	OCR1C = 0xFF; // Top

	TCCR1B |= T1_PRESCALE_BITS(param.t1_prescale);
}

/* 
 * 1 2 Motor lines
 * L H CCW
 * H L CW
 * H H Short break (??)
 * L L Stop 
 */

#define LIMIT(var,limit) \
  if ( (var) > (limit) ) \
    var = limit; \
  else if ( (var) < -(limit) ) \
    var = -limit

typedef enum pin_state_e { l=false, h=true } pin_state_t;
void motor_set(motor_t *motor, motor_speed_t speed) {
	/*
	motor_t curr_motor = motor_list[motor];
	curr_motor.set_speed(&curr_motor,abs(speed));
	curr_motor.set_dir(&curr_motor,sign(speed));
	*/
	motor_uspeed_t abs_speed = (motor_uspeed_t) speed;
	if (abs_speed > MOTOR_SPEED_MAX) abs_speed=MOTOR_SPEED_MAX;

	// atomic
	*(motor->reg_pwmh) = (abs_speed >> 8);
	*(motor->reg_pwm)  = (0x00FF & abs_speed);
	
	if      (speed > 0) {
		motor_pins(motor,h,l);
	}
	else if (speed < 0) {
		motor_pins(motor,l,h);
	}
	else { //if (speed == 0)
		motor_pins(motor,l,l);
	}
}

motor_speed_t motor_get(motor_t *motor) {
	// atomic
	motor_speed_t sp = *(motor->reg_pwm);
	sp += *(motor->reg_pwmh) << 8;
	
	return sp;
}

static inline void motor_pins(motor_t *motor, pin_state_t dir1, pin_state_t dir2) {
  motor_pin(motor,1,dir1);
  motor_pin(motor,2,dir2);
}

static inline void motor_pin(motor_t *motor, uint8_t pin, pin_state_t dir) {
  if (dir == h)
    motor_pin_h(motor,pin);
  else //if (dir==l)
    motor_pin_l(motor,pin);
}

static inline void motor_pin_h(motor_t *motor, pin_state_t pin) {
  /*  
  volatile uint8_t * const port = motor->pin[pin-1].port;
  const uint8_t mask  = motor->pin[pin-1].mask;
  *port |= mask;
  */
  *(motor->pin[pin-1].port) |= motor->pin[pin-1].mask;
}

static inline void motor_pin_l(motor_t *motor, pin_state_t pin) {
  *(motor->pin[pin-1].port) &= (uint8_t) ~(motor->pin[pin-1].mask);
}

