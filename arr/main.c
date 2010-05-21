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

void process_msg(void)
{
	char buf[32];
	uint8_t i;
	for(i = 0; ; i++) {
		if (i > sizeof(buf)) {
			// exceeded buffer size.
			// means that process_msg was called when
			// the rx buffer does not contain a complete message.
			printf("process error\n");
			return;
		}

		int c = getchar();
		if (c == '\0' || c == EOF || c == '\n') {
			printf("got end of msg ");
			switch(c) {
			case '\0':
				printf("null");
				break;
			case EOF:
				printf("eof");
				break;
			case '\n':
				printf("newline");
				break;
			}
			printf("\n");

			buf[i] = '\0';
			break;
		} else {
			buf[i] = c;
		}
	}

	switch(buf[0]) {
	case 's': {
		// servo command
		int pos, num;
		int ret = sscanf(buf+1,":%d:%d",&num,&pos);
		if (ret == 2) {
			printf("setting servo %d to %d\n",num,pos);
			servo_set(num,pos);
		}
	} break;
	default:
		// unknown command
		printf("unknown command \"%s\"\n",buf);
	}
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

