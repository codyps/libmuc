#include "defines.h"
#include "motor.h"
#include "timers.h"
#include <stdio.h>
#include <inttypes.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#if DEBUG_L(1)
#define error_invalid_motor(_m) printf_P(PSTR("\n[error] Motor: Invalid Motor Number: %d [%s]"),_m,__LINE__)
#else
#define error_invalid_motor(_m) 
#endif

uint16_t motor_get_speed(uint8_t motor) {
	uint16_t temp;
	if		(motor==LEFT)
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		temp=MOTOR_LEFT;
	}
	else if	(motor==RIGHT)
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		temp=MOTOR_RIGHT;
	}
	else {
		error_invalid_motor(motor);
		temp=0;
	}
	return temp;
}

void motor_set_speed(uint16_t speed, uint8_t motor) {
	if		(motor==LEFT)
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		MOTOR_LEFT=speed;
	}
	else if (motor==RIGHT)
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		MOTOR_RIGHT=speed;
	}
	else 
		error_invalid_motor(motor);
}

uint8_t motor_mode(motor_mode_t mode, uint8_t motor) {
	static uint8_t c_modes[2]; //=0;
	uint8_t M_IN1,M_IN2;
	uint8_t * c_mode;
	if (motor==LEFT) {
		M_IN1=M_LIN1;
		M_IN2=M_LIN2;
		c_mode=&c_modes[LEFT];
	}
	else if (motor==RIGHT){
		M_IN1=M_RIN1;
		M_IN2=M_RIN2;
		c_mode=&c_modes[RIGHT];
	}
	else {
		error_invalid_motor(motor);
		return MOTOR_MODE_ERROR;
	}
	
	if 	(mode == MOTOR_MODE_CCW ) {
		MOTOR_CTL_PORT&=~(1<<M_IN1); // IN1 = L, IN2 = H
		MOTOR_CTL_PORT|=(1<<M_IN2);
		*c_mode = mode;
	}
	else if (mode == MOTOR_MODE_CW  ) {
		MOTOR_CTL_PORT|=(1<<M_IN1);
		MOTOR_CTL_PORT&=~(1<<M_IN2); // IN1 = H, IN2 = L
		*c_mode = mode;
	}
	else if (mode == MOTOR_MODE_STOP) {
		MOTOR_CTL_PORT&=~((1<<M_IN1)|(1<<M_IN2)); // IN1 = L, IN2 = L
		*c_mode = mode;
	}
	else if (mode == MOTOR_MODE_SB	) {
		MOTOR_CTL_PORT|=(1<<M_IN1)|(1<<M_IN2); // IN1 = H, IN2 = H
		*c_mode = mode;
	}
	
	#if DEBUG_L(2)
	char mname;
	if (motor==LEFT)
		mname='L';
	else
		mname='R';
	printf("\nMotor: %c mode: %d",mname,*c_mode);
	#endif
	
	return *c_mode;
}

uint16_t inc_limit(uint16_t * org, uint16_t inc, uint16_t lim) {
	uint16_t space_left = lim-(*org);
	if (inc>space_left) {
		*org+=space_left;
		//*org=max;
		return (inc-space_left);
	}
	else {
		*org+=inc;
		return 0;
	}
}

uint16_t dec_limit(uint16_t * org, uint16_t dec, uint16_t lim) {
	uint16_t space_left = (*org)-lim;
	if (dec>space_left) {
		*org-=space_left;
		//*org=min;
		return (dec-space_left);
	}
	else {
		*org-=dec;
		return 0;
	}
}

void motor_turn_inc(uint16_t inc,int8_t dir) {
	uint16_t mr = motor_get_speed(RIGHT);
	uint16_t ml = motor_get_speed(LEFT);
	if		(dir==POS)
		dec_limit(&mr,inc_limit(&ml,inc,LF_MAX_SPEED),LF_MIN_SPEED);
	else if (dir==NEG)
		dec_limit(&ml,inc_limit(&mr,inc,LF_MAX_SPEED),LF_MIN_SPEED);
	motor_set_speed(mr,RIGHT);
	motor_set_speed(ml,LEFT);
}

void motor_speed_full(void) {
	motor_set_speed(LF_MAX_SPEED,LEFT);
	motor_set_speed(LF_MAX_SPEED,RIGHT);
	motor_mode(MOTOR_L_FWD,LEFT);
	motor_mode(MOTOR_L_FWD,RIGHT);
}

void motor_speed_stop(void) {
	motor_mode(MOTOR_MODE_STOP,LEFT);
	motor_mode(MOTOR_MODE_STOP,RIGHT);
	motor_set_speed(LF_MIN_SPEED,LEFT);
	motor_set_speed(LF_MIN_SPEED,RIGHT);
}

void motors_init(void) {
	MOTOR_CTL_DDR|=((1<<M_AIN1)|(1<<M_AIN2)|(1<<M_BIN1)|(1<<M_BIN2));
	motor_speed_stop();
}
