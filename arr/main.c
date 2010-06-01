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
#include "clock.h"

/*
ISR(BADISR_vect){
}
*/

#define NAME "arr"

static const char version_str[] PROGMEM = NAME VERSION".\n" ;

static void init(void)
{
	power_all_disable();

	usart_init();
	servo_init();

	sei();
  
	fputs_P(version_str,stdout);
}

static bool sem_trydec(volatile uint8_t *sem)
{
	bool ret = false;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (*sem) {
			(*sem)--;
			ret = true;
		}
	}
	return ret;
}

static bool process_servo_cmd(char *msg)
{
	switch(msg[0]) {
	case 's': {
		int pos, num;
		int ret = sscanf(msg+1," %d %d",&num,&pos);
		if (ret == 2) {
			if (servo_set(num,TICKS_US(pos))) {
				printf(" error.\n");
			}
			return true;
		} else {
			return false;
		}
	}

	case 'S': {
		int pos, num;
		int ret = sscanf(msg+1," %d %d",&num,&pos);
		if (ret == 2) {
			if (servo_set(num,pos)) {
				printf(" error.\n");
			}
			return true;
		} else {
			return false;
		}
	}

	case 'c':
		printf("%d\n",servo_ct());
		return true;

	case 'q': {
		int num;
		int ret = sscanf(msg+1," %d",&num);
		if (ret == 1) {
			printf("%d\n",US_TICKS(servo_get(num)));
			return true;
		} else {
			return false;
		}
	}

	case 'Q': {
		int num;
		int ret = sscanf(msg+1," %d",&num);
		if (ret == 1) {
			printf("%d\n",servo_get(num));
			return true;
		} else {
			return false;
		}
	}

	default:
		printf_P(PSTR("invalid servo command.\n"));
		return true;
	}
}

static void process_msg(void)
{
	// TODO: the size of buf should be the length of the input queue.
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
			buf[i] = '\0';
			break;
		} else {
			buf[i] = c;
		}
	}

	if(i == 0)
		return;

	switch(buf[0]) {
	case 'u':
		fputs_P(version_str,stdout);
		break;
	case '?':
	case 'h':
		printf_P(PSTR("commands:\n"
		              "  h -- prints this.\n"
			      "  ss <sn> <val> -- set servos.\n"
			      "  sq <sn> -- query servos (uS).\n"
			      "  s{S,Q} -- \" \" (ticks).\n"
			      "  sc -- get servo count.\n"
			      "  c -- clear.\n"
			      "  e{+,-,} -- echo ctrl.\n"
			      "  u -- version.\n"));
		break;
	
	case 's':
		if(process_servo_cmd(buf+1))
			break;
		goto invalid_arg;
	case 'c':
		printf("\e[H\e[2J");
		break;
	case 'e':
		switch(buf[1]) {
		case '+':
			usart_echo = 1;
			break;
		case '-':
			usart_echo = 0;
			break;
		default:
			usart_echo ^= 1;
		}
		break;
	default:
		printf("unknown command \"%s\".\n",buf);
		break;
	}
	return;

	invalid_arg:
		printf("bad args for \"%s\".\n",buf);
}

__attribute__((noreturn)) void main(void)
{
	init();
	for(;;) {
		if (sem_trydec(&usart_msg)) {
			process_msg();
		}
	}
}

