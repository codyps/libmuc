#include <string.h>
#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "error_led.h"
#include "frame_async.h"

#include "common.h"

__attribute__((noreturn))
void main(void)
{
	static uint8_t ct;
	cli();
	frame_init();
	led_init();
	sei();
	for(;;) {
		uint8_t buf[16];
		uint8_t len = frame_recv_copy(buf, sizeof(buf));
		if (len) {
			led_flash(5);
			ct = 0;
			frame_send(buf, MIN(len, sizeof(buf)));
			frame_recv_next();
		} else {
			ct++;
			if (ct == 0) {
				ct++;
				led_flash(3);
				const char rdy_str[] = "hello";
				frame_send(rdy_str, strlen(rdy_str));
			}
			_delay_ms(70);
		}

	}
}
