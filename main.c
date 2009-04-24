/*
 * Axon (atmega640)
 	general initialization and main loop
 */

#include "defines.h"

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#include "util.h"

#include "usart1.h"
#include "servo.h"
#include "i2c.h"
#include "i2c_HMC6343.h"

#include "queue.h"

void init(void) {
	power_all_disable();

	usart1_init();
	twi_init();
	hmc6343_init_static();

	servo_init();
	sei();

	DDRB |= (1<<6); // PB6 = status LED
	debug_led_on;
	debug_led_off;

	DDRG &= ~(uint8_t)(1<<5); //PG5 = button
	PORTG|= (1<<5);	// Needs pullups. Low when pressed.

	printf_P(PSTR("\n\n[main init done]\n\n"));
}

void print_bin(uint8_t inp, FILE * stream) {
	for(int8_t j=7; j>=0; --j) {
	   	fputc(((inp&(1<<j))>>j)+'0',stream);
	}
}

ISR(BADISR_vect){
	fprintf_P(io_isr,PSTR("\n[error] bad isr\n"));
	/*
	led_d(0);

	for(int8_t i = 5; i>0; i--) {
		_delay_ms(100);
		led_d(1);
		_delay_ms(100);
		led_d(0);
	}
	*/
}

int main(void) {
	init();

	//i2c_start_xfer();
//	queue_t * rx_q = _get_queue(1);

	for(;;) {
		/*
		if (head_data_updated == true) {
			head_data_updated = false;
			fprintf_P(stdout,PSTR("\nhead:%u pitch:%d roll:%d\n"),\
				head.head,head.pitch,head.roll);
			_delay_ms(50);
			i2c_start_xfer();
		}
		*/

		if (usart_msg) {
			uint16_t index, position;
			--usart_msg;

            /*
			int c;
			do{
                // get characters untill we approach an 'attention' code
                c = fgetc(stdin);
			} while (c != '#');
            */

			int ret = scanf("s %u %u\n",&index, &position);
			if (ret>0) {
				if ( 0 == servo_set(index,CLICKS_US(position))) {
					printf_P(PSTR("ACK s %d %d\n"),index,position);
				}
				else {
					printf_P(PSTR("[input] servo error: servo %d != %d\n"),index,position);
				}
			}
			else {
				puts_P(PSTR("[input] invalid: \"s index position\"\n"));
				usart1_flush_rx();
			}
		}

		_delay_ms(1000);
	}
	return 0;
}

