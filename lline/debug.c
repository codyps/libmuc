#include "debug.h"
#include <util/delay.h>

static void debug_led_flash(uint16_t len,uint16_t out) {
	// no lead in, depend on lead out of previous call.
	
	// blink
	debug_led_on();
	_delay_ms(len);
	debug_led_off();

	// lead out
	_delay_ms(out);
}

