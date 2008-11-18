#include "defines.h"
#include "motor.h"
#include "timer.h"
#include <stdio.h>
#include <inttypes.h>
#include <util/atomic.h>


uint16_t get_motor_L(void) {
	uint16_t temp;
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		temp=MOTOR_LEFT;
	}
	return temp;
}

uint16_t get_motor_R(void) {
	uint16_t temp;
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		temp=MOTOR_RIGHT;
	}
	return temp;
}

void set_motor_L(uint16_t speed) {
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		MOTOR_LEFT=speed;
	}
}

void set_motor_R(uint16_t speed) {
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		MOTOR_RIGHT=speed;
	}
}

uint8_t motor_mode_L(uint8_t mode) {
	static uint8_t c_mode; //=0;

	if 	(mode == MOTOR_MODE_CCW ) {
		MOTOR_CTL_PORT&=~(1<<M_LIN1); // IN1 = L, IN2 = H
		MOTOR_CTL_PORT|=(1<<M_LIN2);
		c_mode = mode;
	}
	else if (mode == MOTOR_MODE_CW  ) {
		MOTOR_CTL_PORT|=(1<<M_LIN1);
		MOTOR_CTL_PORT&=~(1<<M_LIN2); // IN1 = H, IN2 = L
		c_mode = mode;
	}
	else if (mode == MOTOR_MODE_STOP) {
		MOTOR_CTL_PORT&=~((1<<M_LIN1)|(1<<M_LIN2)); // IN1 = L, IN2 = L
		c_mode = mode;
	}
	else if (mode == MOTOR_MODE_SB	) {
		MOTOR_CTL_PORT|=(1<<M_LIN1)|(1<<M_LIN2); // IN1 = H, IN2 = H
		c_mode = mode;
	}
//	if	(mode == MOTOR_MODE_GET )
//		return c_mode;
	printf("motor L mode: %d\n",c_mode);
	return c_mode;
}

uint8_t motor_mode_R(uint8_t mode) {
	static uint8_t c_mode; //=0;

	if 	(mode == MOTOR_MODE_CCW ) {
		MOTOR_CTL_PORT&=~(1<<M_RIN1); // IN1 = L, IN2 = H
		MOTOR_CTL_PORT|=(1<<M_RIN2);
		c_mode = mode;
	}
	else if (mode == MOTOR_MODE_CW  ) {
		MOTOR_CTL_PORT|=(1<<M_RIN1);
		MOTOR_CTL_PORT&=~(1<<M_RIN2); // IN1 = H, IN2 = L
		c_mode = mode;
	}
	else if (mode == MOTOR_MODE_STOP) {
		MOTOR_CTL_PORT&=~((1<<M_RIN1)|(1<<M_RIN2)); // IN1 = L, IN2 = L
		c_mode = mode;
	}
	else if (mode == MOTOR_MODE_SB	) {
		MOTOR_CTL_PORT|=(1<<M_RIN1)|(1<<M_RIN2); // IN1 = H, IN2 = H
		c_mode = mode;
	}
//	if	(mode == MOTOR_MODE_GET )
//		return c_mode;
	printf("motor r mode: %d\n",c_mode);
	return c_mode;
}

enum {LEFT,RIGHT};
void lf_turn_left_inc(uint16_t inc) {
	uint16_t c_speed [2] = {get_motor_L(),get_motor_R()};
	int32_t speed_diff_L = LF_MAX_SPEED - c_speed[LEFT];
	int32_t speed_diff_R = LF_MAX_SPEED - c_speed[RIGHT];

	if ((c_speed[LEFT] + inc) > LF_MAX_SPEED) {
		set_motor_L(c_speed[LEFT]+speed_diff_L);
		set_motor_R(c_speed[RIGHT]-(inc-speed_diff_L));
	else
		set_motor_L(c_speed[LEFT]+inc);
	//TODO: case when ((c_speed[RIGHT]-inc) < LF_MIN_SPEED)?
		
}
void lf_turn_right_inc(uint16_t inc) {
	uint16_t c_speed [2] = {get_motor_L(),get_motor_R()};
	
}
void lf_max_speed(void) {
	set_motor_L(LF_MAX_SPEED);
	set_motor_R(LF_MAX_SPEED);
}
