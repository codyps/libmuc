/*
	servo implimentation.
		configurable pins/indexes
		currently limited to 8 servos (modification possible to
		support	up to 3*8 = 24) uses the timer in fpwm mode,
		repeating 8 times in the 20ms period servos driven
		on consecutive intervals.

		|    /|     |     |    /|    /|    /|
	TCNT	|   / |     |     |   / |   / |   / |
		|  /  |     |  /| |  /  |  /  |  /  |  .
		| /   |     | / | | /   | /   | /   | .		|
		|/    |/|   |/  | |/    |/    |/    |/    |/    |/    |/
	cycle	0     1     2     3     4     5     6     7     0     1     2
		|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|2.5ms|
		|             20ms                              | 20ms  ...
	outputs	|_                                              |_
	Servo1	| |_____________________________________________| |__________
		|      _                                        |      _
	Servo2	|_____| |_______________________________________|_____| |____
		|            _                                  |
	Servo3	|___________| |_________________________________|____________
		|                  _                            |
	Servo4	|_________________| |___________________________|____________
		|						|
	Assigned| S1  | S2  | S3  | S4	|None |	n   |  n  |  n  | S1  | S2 ...
	Timeslot|

	on each cycle one of the servos is activated
	pulled high on the overflow isr, and low on the compare 'a' isr

TODO:
 - apply offseting for output change delays due to software timing
 - investigate using multiple comparison registers
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include <util/atomic.h>

#include "servo.h"
#include "servo_def.h"
#include "common.h"

/*  Time Defines */
#define SV_PERIOD_US	20000

#define SV_TIMER_PERIOD_US (SV_PERIOD_US/SV_TIMER_CYCLES)
#define SV_TIMER_PERIOD_MS (SV_TIMER_PERIOD_US/1000)

#define SERVO_AMOUNT (sizeof(servo)/sizeof(struct servo_s))

// solve(x * (2e-2 / 8) = 65535, x) => x = 26214000
// we only have 16 bits. prescale as needed.
//#if (F_CPU > 26214000) // 26.214 MHz
//#define SV_TIMER_PS 8
//#else
#define SV_TIMER_PS 1
//#endif

/* externaly called functions */
int8_t servo_set(uint8_t servo_number, uint16_t servo_ticks)
{
	if ((servo_ticks >= TICKS_US(500) && servo_ticks <= TICKS_US(2500))
	                               && servo_number < SERVO_AMOUNT) {
		servo[servo_number].pos = servo_ticks;
		return 0;
	}
	return -1;
}

// TODO: make constant.
uint8_t servo_ct(void)
{
	return SERVO_AMOUNT;
}

uint16_t servo_get(uint8_t servo_number)
{
	if (servo_number < SERVO_AMOUNT)
		return servo[servo_number].pos;
	else
		return 0;
}

static void servo_timer_init(void);
static void servo_pin_init(void);

void servo_init(void)
{
	servo_pin_init();
	servo_timer_init();
}

/* internaly called functions */

/* XXX: These are very costly operations (around 46 ops on each call). Need to
 * replace with logic statments, and those need to be automagicaly generated
 * Essentially, convertion of the servo datastructure into code is needed.
 */
#define SERVO_PIN_LOW(_servo_)	\
		( *servo[_servo_].port &= (uint8_t) ~(servo[_servo_].mask) )

#define SERVO_PIN_HIGH(_servo_) \
		( *servo[_servo_].port |= (uint8_t)  (servo[_servo_].mask) )

static void servo_pin_init(void)
{

	// set the pins as outputs, low
	// (port-1)=ddr, (port-2)=pin
	for (uint8_t i = 0 ; i < SERVO_AMOUNT; i++) {
		// port - 1 == DDR
		SERVO_PIN_LOW(i);
		*(servo[i].port - 1) |= (uint8_t)(servo[i].mask); // output
	}
}

/*
struct servo_ctrl {
	TCCRA,
	TCCRB,
	OCRA,
	ICR,
	TCNT,
}
*/

static void servo_timer_init(void)
{
	power_timer_S_enable();

	// Fast PWM, ICR = TOP
	// WGM[3,2,1,0] = 1,1,1,0
	//(disables timer, CS[2,1,0] = 0)
	SERVO_TCCRB = (uint8_t)(1<<WGM3)|(1<<WGM2);
	//(disables outputs, COM[A,B,C][1,0] = 0)
	SERVO_TCCRA = (uint8_t)(1<<WGM1)|(0<<WGM0);

	// write the 16 bit registers.
	uint16_t timer_ticks = TICKS_US(SV_TIMER_PERIOD_US);
	SERVO_ICR  = timer_ticks;
	// causes OVF_vect execution as soon as soon as the timer is enabled.
	SERVO_TCNT = timer_ticks;

	// set the first cycle's position (we don't get the isr for time - 1)
	SERVO_OCRA = servo[0].pos;

	// Clear interrupt flags.
	SV_TIFR = (uint8_t)(1<<OCFA)|(1<<TOV);

	// Enable timer interrupts.
	//SERVO_TIMSK = (1<<OCIEA)|(1<<OCIEB)|(1<<OCIEC)|(1<<TOIE);
	SERVO_TIMSK = (uint8_t)(1<<OCIEA)|(1<<TOIE);

	// Presccale & Start.
	SERVO_TCCRB|= TIMER_PRESCALE_1;
}


static inline void servo_cmpA_isr_off(void)
{
	SERVO_TIMSK &= (uint8_t) ~(1<<OCIEA);
}

static inline void servo_cmpA_isr_on(void)
{
	SERVO_TIMSK |= (uint8_t) (1<<OCIEA);
}

static uint8_t cycle; //= 0;
static uint8_t old_cycle;

// Needs to spend less than 600us, F_CPU/1000/10*6 clicks. (16e3@16e6Hz)
ISR(TIMER_S_OVF_vect)
{
	if (cycle >= SERVO_AMOUNT) {
		// these servos don't exsist, delaying until next
		// 20ms period.
		/* TODO: could change the timer-top to cause only one OVF isr to
		 * occur instead of 10 - SERVO_AMOUNT */
		servo_cmpA_isr_off();
	} else {
		// set servo 'cycle' pin(s) high
		SERVO_PIN_HIGH(cycle);
		servo_cmpA_isr_on();
	}

	// Set OCRA for the following cycle.
	old_cycle = cycle;
	cycle ++;
	if (cycle >= (SV_TIMER_CYCLES))
		cycle = 0;

	// Assignment must occour before the TCNT hits BOTTOM.
	// That will always occour after the COMPA_vect executes.
	// And interrupts need to be enabled with COMPA_vect executes.
	if (cycle >= SERVO_AMOUNT)
		SERVO_OCRA = 0xFFFF;
	else
		SERVO_OCRA = servo[cycle].pos;
}

ISR(TIMER_S_COMPA_vect)
{
	// Limit is 100 us, 1600 clicks
	SERVO_PIN_LOW(old_cycle);
}

/*
ISR(TIMER_S_COMPB_vect)
{
}

ISR(TIMER_S_COMPC_vect)
{
}
*/
