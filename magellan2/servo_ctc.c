#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>

#include <util/atomic.h>

#include "servo.h"
#include "util.h"


uint16_t servo_position [SERVO_AMOUNT];
volatile uint8_t * servo_port [SERVO_AMOUNT];
uint8_t servo_index [SERVO_AMOUNT];

uint8_t comp5_servos[TIMER5_COMP_REGS][SERVO_AMOUNT];


void timer5_fpwm_start(void);
void servo_pin_init(void);

// Value max = 16000, 0x3e80
uint8_t servo_set(uint16_t servo_val, uint8_t servo_number) {
	if (servo_val>SERVO_MAX) 
		return -1;
	servo_position[servo_number] = servo_val;
}

void init_servos(void) {
	servo_pin_init();	
	memset_16( servo_values, SERVO_MAX/2, SERVO_AMOUNT);
	timer5_fpwm_start();
}

void servo_pin_init(void) {	

	servo_port [SERVO_P]   = &SERVO_P_PORT;
	servo_index[SERVO_P]   = &SERVO_P_INDEX;

	servo_port [SERVO_T]   = &SERVO_T_PORT;
	servo_index[SERVO_T]   = &SERVO_T_INDEX;	

	servo_port [SERVO_IRL] = &SERVO_IRL_PORT;
	servo_index[SERVO_IRL] = &SERVO_IRL_INDEX;

	servo_port [SERVO_IRR] = &SERVO_IRR_PORT;
	servo_index[SERVO_IRR] = &SERVO_IRR_INDEX;
}

void timer5_init(void) {
	power_timer5_enable();	

	// CTC, ICR5 = TOP
	//WGM5[3,2,1,0] = 1,1,1,0
	TCCR5B = (1<<WGM3)|(1<<WGM2); //(disables timer, CS[2,1,0] = 0		)
	TCCR5A = (0<<WGM1)|(0<<WGM0); //(disables outputs, COM[A,B,C][1,0] = 0	)
	
	TIMSK5 = (1<<ICIE5);
	// write the 16 bit registers.
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ICR5 = CLICKS_MS(1);	
		TCNT5 = 0;
	
		//OCR5A = ?;
		//OCR5B = ?;
		//OCR5C = ?;
	};

	// Prescale & Start.
	TCCR5B|= TIMER5_1_PRESCALE;
}

void next_servo(void) {
	static next_servo = 0;
	ICR5 = 
}


ISR(TIMER5_CAPT_vect) {
	
	
}

