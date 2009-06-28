#ifndef _DEBUG_H_
#define _DEBUG_H_

#define debug_led_off	PORTB |= (1<<6)
#define debug_led_on	PORTB &= (uint8_t)~(1<<6)

#define led_d(state)	if (state==0)		\
				debug_led_off;	\
			else			\
				debug_led_on;	

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
