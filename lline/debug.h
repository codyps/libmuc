#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "defines.h"
#include "common.h"
#include <stdint.h>
#include <util/delay.h>
#include <avr/io.h>

#define DEBUG_LED_P	A
#define DEBUG_LED_I 4

#define DEBUG_LED_PIN  PIN(DEBUG_LED_P)
#define DEBUG_LED_DDR  DDR(DEBUG_LED_P)
#define DEBUG_LED_PORT PORT(DEBUG_LED_P)

#define debug_led_off()	DEBUG_LED_PORT |= (1<<DEBUG_LED_I)
#define debug_led_on()	DEBUG_LED_PORT &= (uint8_t)~(1<<DEBUG_LED_I)
#define debug_led_flip() DEBUG_LED_PIN  |= (1<<DEBUG_LED_I)
#define debug_led_init() DEBUG_LED_DDR  |= (1<<DEBUG_LED_I)

inline static void debug_led_flash(uint16_t len,uint16_t out) {
	// no lead in, depend on lead out of previous call.
	
	// blink
	debug_led_on();
	_delay_ms(len);
	debug_led_off();

	// lead out
	_delay_ms(out);
}


#define led_d(state)				\
			if (state==0)			\
				debug_led_off();	\
			else					\
				debug_led_on()

/* Debuging */
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

#endif /* _DEBUG_H_ */
