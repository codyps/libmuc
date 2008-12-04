#include "defines.h"
#include "motor.h"
#include "timers.h"
#include <stdio.h>
#include <inttypes.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#define error_invalid_motor(_m) printf_P(PSTR("\n[error] Motor: Invalid Motor Number: %d [%s]"),_m,__LINE__)
#define mr MOTOR_RIGHT
#define ml MOTOR_LEFT
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

uint8_t motor_mode(uint8_t mode, uint8_t motor) {
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

	char* mname;
	if (motor==LEFT)
		mname="Left";
	else
		mname="Right";
	printf("\nMotor: %s mode: %d",mname,*c_mode);
	return *c_mode;
}

uint16_t inc_limit(uint16_t * org, uint16_t inc, uint16_t lim) {
	uint16_t space_left = lim-(*org);
	uint16_t ret = (inc-space_left);
	if (ret>0) {
		*org+=space_left;
		return ret;
	}
	else {
		*org+=inc;
		return 0;
	}
}
void lf_turn_inc(uint16_t inc,int8_t dir) {
	if		(dir>0)
		mr-=inc_limit(&ml,inc,0xFFFF);
	else if (dir<0)
		ml-=inc_limit(&mr,inc,0xFFFF);
}

void lf_full_speed(void) {
	motor_set_speed(LF_MAX_SPEED,LEFT);
	motor_set_speed(LF_MAX_SPEED,RIGHT);
}
