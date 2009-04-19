/*
Globaly Utilized Information
*/


#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define F_CPU 8000000

// Debuging 
#define debug
#define DEBUG 2

#define DEBUG_L(LEVEL) (DEBUG>=LEVEL)

#if DEBUG_L(1) 
	#define dpf_P(...) printf_P(__VA_ARGS__)
	#define dpf(...) printf(__VA_ARGS__)
#else
	#define dpf_P(...)  
	#define dpf(...)  
#endif


/* Amount Defines for Line Following */
#define LF_INC_LARGE 0x0700
#define LF_INC_SMALL 0x0200
#define LF_INC_INTEG 0x0020
#define LF_INTEG_MAX 0x1000

#define LF_MAX_SPEED 0xc000	// Software implimentation sucks. anything close to the 0 or ffff
#define LF_MIN_SPEED 0x0500 // has undefined behavior.


/* Mode/State */
typedef enum {WAIT,TEST,FOLLOW} main_mode_t;
main_mode_t volatile c_mode;
bool volatile initial;

#endif
