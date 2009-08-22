/*
	Low level motor control via timer based pwm.
*/

#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>


#include "motor_conf.h"

void motor_init(void);
static void motor_pwm10_init(void);

void motor_init(void) {
	motor_pwm10_init();
}

static void motor_pwm10_init(void) {
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

	TCCR1A = (USE_OC1A<<COM1A1) | (0<<COM1A0) | // Output pin modes (OCR1A)
		(USE_OC1B<<COM1B1) | (0<<COM1B0) | // ^^               (OCR1B)
		(       0<<FOC1A ) | (0<<FOC1B ) | // Force output compare
		(USE_OC1A<<PWM1A ) |
		(USE_OC1B<<PWM1B ) ;

	/*
	TCCR1B = (0<<PWM1X ) | // PWM inversion (switch !OCR1x and OCR1x)
		(0<<PSR1  ) | // Prescaler reset, cleared by hw
		(0<<DTPS11) | (0<<DTPS10); // Dead time prescaler bits
		//CS1{3,2,1,0}: prescale bits
	*/
		
	TCCR1C = (USE_OC1A<<COM1A1S)|(0<<COM1A0S) | // Shadow bits
		(USE_OC1B<<COM1B1S)|(0<<COM1B0S) | // ^^
		(USE_OC1D<<COM1D1) | (0<<COM1D0) | // Output pin modes D
		(0<<FOC1D ) | // Force output compare
		(USE_OC1D<<PWM1D ) ;

	TCCR1D = (0<<FPIE1) | // Fault protection interrupt enable
		(0<<FPEN1) | // FP enable
		(0<<FPNC1) | // FP noise canceller
		(0<<FPES1) | // FP edge select
		(0<<FPAC1) | // FP analog comparitor enable
		(0<<FPF1 ) | // FP interrupt flag
		(0<<WGM11) | (1<<WGM10); // Waveform select
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

	TCCR1B |= T1_PRESCALE_BITS(MOTOR_T1_PRESCALE);
}

#define H 1
#define L 2

#define MOTOR_PIN(motor,port,dir) \
	MOTOR_PIN_##dir(motor,port)	

#define MOTOR_PIN_H(motor,port) \
	*(motor_list[motor].port_p##port) |= motor_list[motor].mask_p##port

#define MOTOR_PIN_L(motor,port) \
	*(motor_list[motor].port_p##port) &= (uint8_t) ~(motor_list[motor].mask_p##port)
/* 
 * 1 2 Motor lines
 * L H CCW
 * H L CW
 * H H Short break (??)
 * L L Stop 
 */

void motor_set(uint8_t motor, motor_speed_t speed) {
	/*
	motor_t curr_motor = motor_list[motor];
	curr_motor.set_speed(&curr_motor,abs(speed));
	curr_motor.set_dir(&curr_motor,sign(speed));
	*/
	motor_uspeed_t abs_speed = (motor_uspeed_t) speed;

	// atomic
	*(motor_list[motor].reg_pwmh) = (uint8_t) (abs_speed >> 8);
	*(motor_list[motor].reg_pwm ) = (0x00FF & abs_speed);
	
	if      (speed > 0) {
		MOTOR_PIN(motor,1,H);
		MOTOR_PIN(motor,2,L);
	}
	else if (speed < 0) {
		MOTOR_PIN(motor,1,L);
		MOTOR_PIN(motor,2,H);
	}
	else { // if (speed == 0)
		MOTOR_PIN(motor,1,L);
		MOTOR_PIN(motor,2,L);
	}
}

motor_speed_t motor_get(uint8_t motor) {
	// atomic
	motor_speed_t sp = *(motor_list[motor].reg_pwm);
	sp += *(motor_list[motor].reg_pwmh) << 8;
	
	return sp;
}

