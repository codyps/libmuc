#ifndef _DEFINES_H_
#define _DEFINES_H_ 

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>


#define F_CPU 8000000

/* Debuging */
#define DEBUG_LED_PIN	PINB
#define DEBUG_LED_PORT	PORTB
#define DEBUG_LED_DDR	DDRB
#define DEBUG_LED_POS	4
#define DEBUG_LED_FLIP DEBUG_LED_PIN|=(1<<DEBUG_LED_POS); 
#define DEBUG_LED_ON DEBUG_LED_PORT|=(1<<DEBUG_LED_POS); 

#define DEBUG 1
#define DEBUG_L(LEVEL) (DEBUG>=LEVEL)

#if DEBUG_L(1) 
	#define dpf_P(...) printf_P(__VA_ARGS__)
	#define dpf(...) printf(__VA_ARGS__)
#else
	#define dpf_P(...)  
	#define dpf(...)  
#endif
#if DEBUG_L(2)
	#define dpfv(...) printf(__VA_ARGS__)
	#define dpfv_P(...) printf_P(__VA_ARGS__)
#else
	#define dpfv(...)
	#define dpfv_P(...)
#endif
#if DEBUG_L(3)
	#define dpfV(...) printf(__VA_ARGS__)
	#define dpfV_P(...) printf_P(__VA_ARGS__)
#else
	#define dpfV(...)
	#define dpfV_P(...)
#endif

/* Linefollowing Mode/State */
typedef enum {WAIT,TEST,FOLLOW} main_mode_t;
main_mode_t volatile c_mode;
bool volatile initial;

#endif

