#include <string.h>
#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "error_led.h"
#include "frame_async.h"

__attribute__((noreturn))
void main(void)
{
	static uint8_t ct;
	cli();
	frame_init();
	led_init();
	sei();
	for(;;) {
		uint8_t *f = frame_recv();
		if (f) {
			led_flash(5);
			ct = 0;
			uint8_t flen = frame_recv_len();
			frame_send(f, flen);
			frame_recv_drop();
		} else {
			ct++;
			_delay_ms(20);
		}

		if (ct == 0) {
			ct++;
			led_flash(3);
			const char rdy_str[] = "hello";
			frame_send(rdy_str, strlen(rdy_str));
		}
	}
}
