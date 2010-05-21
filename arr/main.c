/*
 * Arduino
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <util/atomic.h>
#include <util/parity.h> 
#include <util/delay.h>

#include "servo.h"
#include "usart.h"

/*
ISR(BADISR_vect){
}
*/

void init(void)
{
	power_all_disable();

	usart_init();
	servo_init();

	sei();
  
	printf_P(PSTR("\n\n[main init done]\n\n"));
}

bool static sem_trydec(uint8_t sem)
{
	bool ret = false;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (sem) {
			sem--;
			ret = true;
		}
	}
	return ret;
}

int main(void)
{
	init();
	for(;;) {
		if (sem_trydec(usart_msg)) {
			int x = getchar();
		}
	}
   	return 0;
}

