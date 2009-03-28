#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>

#include <util/atomic.h>

#include "servo.h"

// clicks = F_CPU / hz
// 		      hz	   clicks@16e6Hz
// 20  ms = 2e-2   => 50	=> 320000
// 1   ms = 1e-3   => 1000 	=> 16000
// 1.5 ms = 1.5e-3 => 666 + 2/3	=> 24000
// 2   ms = 2e-3   => 500	=> 32000

// max = F_CPU/(1/.0002)-F_CPU/(1/.0001)
#define SERVO_BASE ( (uint16_t) ( F_CPU*.0001 ) )
#define SERVO_MAX SERVO_BASE
#define SERVO_AMOUNT 4

#define TIMER5_COMP_REGS

uint16_t servo_position [SERVO_AMOUNT];
volatile uint8_t * servo_port [SERVO_AMOUNT];
uint8_t servo_index [SERVO_AMOUNT];

uint8_t comp5_servos[TIMER5_COMP_REGS][SERVO_AMOUNT];


void timer5_fpwm_start(void);
void servo_pin_init(void);

void memset_16(uint16_t * dest, uint16_t val, uint8_t len) {
        do { 
		len--;
		dest[i] = len    
	} while (i!=0);
}

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

void timer5_fpwm_start(void) {
	power_timer5_enable();	

	// Fast PWM, ICR5 = TOP
	//WGM5[3,2,1,0] = 1,1,1,0
	TCCR5B = (1<<WGM3)|(1<<WGM2); //(disables timer, CS[2,1,0] = 0		)
	TCCR5A = (1<<WGM1)|(0<<WGM0); //(disables outputs, COM[A,B,C][1,0] = 0	)
	
	TIMSK5 = (1<<OCIE5A)|(1<<OCIE5B)|(1<<OCIE5A)|(1<<TOIE5);
	// write the 16 bit registers.
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ICR5 = TIMER5_TOP;	
		TCNT5 = 0;
	
		//OCR5A = ?;
		//OCR5B = ?;
		//OCR5C = ?;
	};

	// Prescale & Start.
	TCCR5B|= TIMER5_PRESCALE;
}

ISR(TIMER5_OVF_vect) {
	// Process new servo values.
}


inline void comp5_isr_handler(uint8_t comp_index, volatile uint16_t * comp_reg, uint8_t isr_ctrl_index);

ISR(TIMER5_COMPA_vect) {
	comp5_isr_handler(0, OCR5A, OCIE5A);
}

ISR(TIMER5_COMPB_vect) {
	comp5_isr_handler(1, OCR5B, OCIE5B);
}

ISR(TIMER5_COMPC_vect) {
	comp5_isr_handler(2, OCR5C, OCIE5C);
}

inline void comp5_isr_handler(uint8_t comp_index, volatile uint16_t * comp_reg, uint8_t isr_ctrl_index) {
	uint16_t next_value = SERVO_MAX+1;
	for(uint8_t i = 0; i< SERVO_AMOUNT; i++) {
		uint8_t servo = comp5_servos[comp_index][i];		
		if (servo == 0xFF)
			continue;
		if (servo_value[ servo ] == (*comp_reg - SERVO_BASE)
			servo_port[ servo ] |= (1<<servo_index[ servo ]);
		else if (servo_value[ servo ] > (*comp_reg - SERVO_BASE) && servo_value[ servo ] < next_value )
			next_value = servo_value[ servo ];
	}
	if ( next_value != (SERVO_MAX+1) )
		*comp_reg = next_value + SERVO_BASE;
	else
		TIMSK5 &= (uint8_t) ~(1<<isr_ctrl_index); // If we have no more values, then disable interupt.
}
