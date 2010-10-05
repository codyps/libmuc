#include <string.h>
#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include "frame_async.h"

__attribute__((noreturn))
void main(void)
{
	cli();
	frame_init();
	sei();
	static uint8_t ct;
	for(;;) {
		uint8_t *f = frame_recv();
		if (f) {
			ct = 0;
			uint8_t flen = frame_recv_len();
			frame_send(f, flen);
			frame_recv_drop();
		} else {
			ct++;
			_delay_ms(50);
		}

		if (ct == 0xff) {
			ct = 0;
			const char rdy_str[] = "hello";
			frame_send(rdy_str, strlen(rdy_str));
		}
	}
}
