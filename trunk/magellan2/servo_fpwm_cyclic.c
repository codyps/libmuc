/*
	servo implimentation.
		configurable pins/indexes
		currently limited to 8 servos (modification possible to support up to 3*8 = 24)
		uses the timer in fpwm mode, repeating 8 times in the 20ms period
		servos driven on consecutive intervals

				|    /|    /|    /|    /|    /|    /|
	TCNT		|   / |   / |   / |   / |   / |   / |
	(counter)	|  /  |  /  |  /  |  /  |  /  |  /  |  .
				| /   | /   | /   | /   | /   | /   | .
				|/    |/    |/    |/    |/    |/    |/    |/    |/    |/
	cycle		0     1     2     3     4     5     6     7     0     1     2
				|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|
				| 	      20ms	      							| 20ms  ...
	output  	|_                             					 _     
			P	| |_____________________________________________| |__________
				|      _                      					|      _
			T	|_____| |_____________________________________________| |____
				|            _                					|            
			IRL	|___________| |______________________________________________ 
				|                  _          					|
			IRR	|_________________| |________________________________________
				|
				| P   | T   | IRL | IRR	|None |	n	|	n |  n  |

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

// Time Defines
#define TIMER_CYCLES 8

#define SERVO_PERIOD_US	20000

#define TIMER_PERIOD_US (SERVO_PERIOD_US/TIMER_CYCLES)
#define TIMER_PERIOD_MS (TIMER_PERIOD_US/1000)


/* Servo Informaiton and State  */
struct _servo {
	uint16_t pos; // position
	volatile uint8_t * port;
	uint8_t  mask; 
} ;

#define SERVO_INIT(_port,_index) {  .port = &_port, .mask = (uint8_t) 1<<_index, .pos = CLICKS_MS(1) + CLICKS_MS(1)/2 }
struct _servo servo[] = {
	SERVO_INIT(SERVO_P_PORT, SERVO_P_INDEX),
	SERVO_INIT(SERVO_T_PORT, SERVO_T_INDEX),
	SERVO_INIT(SERVO_IRL_PORT, SERVO_IRL_INDEX),
	SERVO_INIT(SERVO_IRR_PORT, SERVO_IRR_INDEX),
};

#define SERVO_AMOUNT (sizeof(servo)/sizeof(struct _servo))

void timer_servo_init(void);
void servo_pin_init(void);

uint8_t servo_set(uint16_t servo_val, uint8_t servo_number) {
	if ( servo_val>CLICKS_US(2200) || servo_val<CLICKS_US(700) || servo_number>SERVO_AMOUNT) 
		return -1;
	servo[servo_number].pos = servo_val;
	return 0;
}

void servo_init(void) {
	servo_pin_init();	
	
	// Do analysis on the servo_positions to place them when more than 5 are being used.
	//update_servos();

	timer_servo_init();
}

// These are very costly operations (around 46 ops on each call)
// Need to replace with logic statments, and those need to be automagicaly generated
#define SERVO_PIN_LOW(_servo_)	\
		( *servo[_servo_].port &= (uint8_t) ~(servo[_servo_].mask) )
		
#define SERVO_PIN_HIGH(_servo_) \
		( *servo[_servo_].port |= (uint8_t)  (servo[_servo_].mask) )


#define max(a,b) (b<a)?a:b
#define min(a,b) (b>a)?a:b

void servo_pin_init(void) {
		
	// set the pins as outputs, low
	// (port-1)=ddr, (port-2)=pin
	for (uint8_t i = 0 ; i < SERVO_AMOUNT; i++) {
		*(servo[i].port-1) |= (uint8_t)    (servo[i].mask)  ; // output
		SERVO_PIN_LOW(i);
	}
}


static uint8_t cycle; //= 0;

void timer_servo_init(void) {
	power_timer_S_enable();	

	// Fast PWM, ICR5 = TOP
	//WGM[3,2,1,0] = 1,1,1,0
	SERVO_TCCRB = (1<<WGM3)|(1<<WGM2); //(disables timer, CS[2,1,0] = 0		)
	SERVO_TCCRA = (1<<WGM1)|(0<<WGM0); //(disables outputs, COM[A,B,C][1,0] = 0	)
	
	//SERVO_TIMSK = (1<<OCIEA)|(1<<OCIEB)|(1<<OCIEC)|(1<<TOIE);
	SERVO_TIMSK = (1<<OCIEA)|(1<<TOIE);
	// write the 16 bit registers.
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		SERVO_ICR = CLICKS_US(TIMER_PERIOD_US);	
		SERVO_TCNT = 0;
	
		SERVO_OCRA = servo[cycle].pos;
	};
	
	// Prescale & Start.
	SERVO_TCCRB|= TIMER_PRESCALE_1;

	// We don't get the first OVF isr (when cycle == 0), so pull it high now.
	SERVO_PIN_HIGH(cycle);
}

// Needs to spend less than 1ms, F_CPU/1000 clicks. (16e3@16e6Hz)
ISR(TIMER_S_OVF_vect) {

	//cycle = (cycle+1)%(TIMER_CYCLES); // to many ops.

	cycle++;
	if ( cycle >= (TIMER_CYCLES) )
		cycle = 0;

	if (cycle < SERVO_AMOUNT) {
		// set servo 'cycle' pin(s) high
		SERVO_PIN_HIGH(cycle);
		// set the OCR5A appropratly
		SERVO_OCRA = servo[cycle].pos;
	}
	else {// if (cycle == 4) {
		SERVO_OCRA = 0xFFFF;
	}
}

ISR(TIMER_S_COMPA_vect) { 
	// Limit is .5 ms, 8000 clicks
	// 96 ops just for this... oi.
	SERVO_PIN_LOW(cycle);
}
/*
ISR(TIMER_S_COMPB_vect) {
	
}

ISR(TIMER_S_COMPC_vect) {
	
}
*/
