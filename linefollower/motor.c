#include "defines.h"
#include "motor.h"
#include "timers.h"
#include <stdio.h>
#include <inttypes.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>

#define error_invalid_motor(_m) printf_P(PSTR("\n[error] Motor: Invalid Motor Number: %d [%s]"),_m,__LINE__)

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


//TODO: This should replace the left/right specific functions.
/*
void lf_turn_inc(int32_t inc) {
	uint16_t c_speed [2] = {get_motor_L(),get_motor_R()};
	//L < 0, R > 0
	if (inc==0)
		return;
	
	int32_t max_d[] = {LF_MAX_SPEED - c_speed[LEFT], LF_MAX_SPEED - c_speed[RIGHT]};
	int32_t min_d[] = {LF_MIN_SPEED - c_speed[LEFT], LF_MIN_SPEED - c_speed[RIGHT]};
	
	
	int32_t c_spin = c_speed[LEFT]-c_speed[RIGHT];
	int32_t spin_goal = c_spin+inc;
	
	if		(spin_goal>INT32_MAX)
		spin_goal=INT32_MAX;
	else if	(spin_goal<INT32_MIN)
		spin_goal=INT32_MIN;
		
	if		(inc<0)
	
	
	
}
*/

// These suck. Fix them.
void lf_turn_left_inc(uint16_t inc) {
	uint16_t c_speed [2] = {motor_get_speed(LEFT),motor_get_speed(RIGHT)};
	int32_t speed_diff_L = LF_MAX_SPEED - c_speed[LEFT];
//	int32_t speed_diff_R = LF_MAX_SPEED - c_speed[RIGHT];

	if ((c_speed[LEFT] + inc) > LF_MAX_SPEED) {
		motor_set_speed(c_speed[LEFT]+speed_diff_L,LEFT);
		if ((c_speed[RIGHT]-inc) < LF_MIN_SPEED)
			motor_set_speed(LF_MIN_SPEED,RIGHT);
		else
			motor_set_speed(c_speed[RIGHT]-(inc-speed_diff_L),RIGHT);
	}
	else
		motor_set_speed(c_speed[LEFT]+inc,LEFT);
}
void lf_turn_right_inc(uint16_t inc) {
	uint16_t c_speed [2] = {motor_get_speed(LEFT),motor_get_speed(RIGHT)};
//	int32_t speed_diff_L = LF_MAX_SPEED - c_speed[LEFT];
	int32_t speed_diff_R = LF_MAX_SPEED - c_speed[RIGHT];

	if ((c_speed[RIGHT] + inc) > LF_MAX_SPEED) {
		motor_set_speed(c_speed[RIGHT]+speed_diff_R,RIGHT);
		if ((c_speed[LEFT]-inc) < LF_MIN_SPEED)
			motor_set_speed(LF_MIN_SPEED,LEFT);
		else
			motor_set_speed(c_speed[LEFT]-(inc-speed_diff_R),LEFT);
	}
	else
		motor_set_speed(c_speed[RIGHT]+inc,RIGHT);
}

void lf_full_speed(void) {
	motor_set_speed(LF_MAX_SPEED,LEFT);
	motor_set_speed(LF_MAX_SPEED,RIGHT);
}
