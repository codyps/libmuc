#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>

#include <util/atomic.h>

#include "servo.h"
#include "util.h"

// Time until overflow (in ms)
#define TIMER_PERIOD 4


/* Servo Informaiton and State (make into a struct?) */
uint16_t servo_position [SERVO_AMOUNT];
volatile uint8_t * servo_port [SERVO_AMOUNT];
uint8_t servo_index [SERVO_AMOUNT];
/* (end) */


void timer5_fpwm_start(void);
void servo_pin_init(void);

uint8_t servo_set(uint16_t servo_val, uint8_t servo_number) {
	if (servo_val>CLICKS_MS(3) || servo_val<CLICKS_MS(1) || servo_number>SERVO_AMOUNT) 
		return -1;
	servo_position[servo_number] = servo_val;
}

void init_servos(void) {
	servo_pin_init();	
	memset_16( servo_values, CLICKS_MS(1)/2+CLICKS_MS(1), SERVO_AMOUNT);
	update_servos();
	timer5_init();
}

//TODO: This should be assigned at compile time, not run time.
void servo_pin_init(void) {	

	servo_port [SERVO_P]   = &SERVO_P_PORT;
	servo_index[SERVO_P]   = SERVO_P_INDEX;

	servo_port [SERVO_T]   = &SERVO_T_PORT;
	servo_index[SERVO_T]   = SERVO_T_INDEX;	

	servo_port [SERVO_IRL] = &SERVO_IRL_PORT;
	servo_index[SERVO_IRL] = SERVO_IRL_INDEX;

	servo_port [SERVO_IRR] = &SERVO_IRR_PORT;
	servo_index[SERVO_IRR] = SERVO_IRR_INDEX;
}

void timer5_init(void) {
	power_timer5_enable();	

	// Fast PWM, ICR5 = TOP
	//WGM5[3,2,1,0] = 1,1,1,0
	TCCR5B = (1<<WGM3)|(1<<WGM2); //(disables timer, CS[2,1,0] = 0		)
	TCCR5A = (1<<WGM1)|(0<<WGM0); //(disables outputs, COM[A,B,C][1,0] = 0	)
	
	//TIMSK5 = (1<<OCIE5A)|(1<<OCIE5B)|(1<<OCIE5A)|(1<<TOIE5);
	TIMSK5 = (1<<OCIE5A)|(1<<TOIE5);
	// write the 16 bit registers.
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ICR5 = CLICKS_MS(TIMER_PERIOD);	
		TCNT5 = 0;
	
		//OCR5A = ?;
		//OCR5B = ?;
		//OCR5C = ?;
	};

	// Prescale & Start.
	TCCR5B|= TIMER_PRESCALE_1;
}

#define SERVO_PIN_LOW(_servo_) servo_port[_servo_] &= (uint8_t) ~(1<<servo_index[_servo_])
#define SERVO_PIN_HIGH(_servo_) servo_port[_servo_] |= (1<<servo_index[_servo_])

#define max(a,b) (b<a)?a:b
#define min(a,b) (b>a)?a:b

static uint8_t cycle; //= 0;

// Needs to spend less than 1ms, F_CPU/1000 ops. (16e3@16e6Hz)
ISR(TIMER5_OVF_vect) {

	cycle = (cycle+1)%(20/TIMER_PERIOD);

	if (cycle != 4) {
		// set servo 'cycle' pin(s) high
		SERVO_PIN_HIGH(cycle);
		// set the OCR5A appropratly
		OCR5A = servo_value[cycle];
	}
	else // if (cycle == 4) {
		//???
	}
}

ISR(TIMER5_COMPA_vect) {
	SERVO_PIN_LOW(cycle);
}

ISR(TIMER5_COMPB_vect) {
	
}

ISR(TIMER5_COMPC_vect) {
	
}

