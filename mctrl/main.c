#include <avr/io.h>
#include <avr/interrupt.h>
#include "frame_async.h"

__attribute__((noreturn))
void main(void)
{
	cli();
	hldc0_init();
	sei();
	for(;;) {

	}
}
