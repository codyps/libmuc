/*
	servo implimentation.
		configurable pins/indexes
		currently limited to 4 servos (modification possible to support 5 or 15)
		uses the timer in fpwm mode, repeating 5 times in the 20ms period
		servos driven on consecutive intervals

	TCNT	|   /|   /|   /|   /|   /|
      (counter)	|  / |  / |  / |  / |  / |
		| /  | /  | /  | /  | /  |
		|/   |/   |/   |/   |/   |/...
	cycle	0    1    2    3    4    0
		|4ms |4ms |4ms |4ms |4ms |
		| 	   20ms		 |

		on each cycle one of the servos is activated
			pulled high on the overflow isr, and low on the compare 'a' isr

*/

#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include <util/atomic.h>

#include "servo.h"
#include "servo_conf.h"
#include "util.h"

// Time until overflow (in ms)
#define TIMER_PERIOD 4


/* Servo Informaiton and State (make into a struct?) */
uint16_t servo_position [SERVO_AMOUNT];
volatile uint8_t * servo_port [SERVO_AMOUNT];
uint8_t servo_index [SERVO_AMOUNT];
/* (end) */


void timer_servo_init(void);
void servo_pin_init(void);

uint8_t servo_set(uint16_t servo_val, uint8_t servo_number) {
	if (servo_val>CLICKS_MS(3) || servo_val<CLICKS_MS(1) || servo_number>SERVO_AMOUNT) 
		return -1;
	servo_position[servo_number] = servo_val;
	return 0;
}

void init_servos(void) {
	servo_pin_init();	
	memset_16( servo_position, CLICKS_MS(1)/2+CLICKS_MS(1), SERVO_AMOUNT);
	//update_servos();
	timer_servo_init();
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


#define SERVO_PIN_LOW(_servo_) ( *servo_port[_servo_] &= (uint8_t) ~( 1<<servo_index[_servo_] ) )
#define SERVO_PIN_HIGH(_servo_) ( *servo_port[_servo_] |= (uint8_t) (1<<servo_index[_servo_]) )

#define max(a,b) (b<a)?a:b
#define min(a,b) (b>a)?a:b

static uint8_t cycle; //= 0;

void timer_servo_init(void) {
	power_timer5_enable();	

	// Fast PWM, ICR5 = TOP
	//WGM5[3,2,1,0] = 1,1,1,0
	SERVO_TCCRB = (1<<WGM3)|(1<<WGM2); //(disables timer, CS[2,1,0] = 0		)
	SERVO_TCCRA = (1<<WGM1)|(0<<WGM0); //(disables outputs, COM[A,B,C][1,0] = 0	)
	
	//TIMSK5 = (1<<OCIEA)|(1<<OCIEB)|(1<<OCIEC)|(1<<TOIE);
	SERVO_TIMSK = (1<<OCIEA)|(1<<TOIE);
	// write the 16 bit registers.
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		SERVO_ICR = CLICKS_MS(TIMER_PERIOD);	
		SERVO_TCNT = 0;
	
		SERVO_OCRA = servo_position[cycle];
	};

	// We don't get the first OVF isr, so pull it high now.
	SERVO_PIN_HIGH(cycle);
	
	// Prescale & Start.
	SERVO_TCCRB|= TIMER_PRESCALE_1;
}

// Needs to spend less than 1ms, F_CPU/1000 ops. (16e3@16e6Hz)
ISR(TIMER_S_OVF_vect) {

	cycle = (cycle+1)%(20/TIMER_PERIOD);

	if (cycle != 4) {
		// set servo 'cycle' pin(s) high
		SERVO_PIN_HIGH(cycle);
		// set the OCR5A appropratly
		SERVO_OCRA = servo_position[cycle];
	}
	else {// if (cycle == 4) {
		//???
	}
}

ISR(TIMER_S_COMPA_vect) {
	SERVO_PIN_LOW(cycle);
}
/*
ISR(TIMER_S_COMPB_vect) {
	
}

ISR(TIMER_S_COMPC_vect) {
	
}
*/
