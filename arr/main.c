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

#include "motor.h"
#include "adc.h"
#include "usart.h"
#include "line.h"
#include "clock.h"
#include "pid.h"
#include "msg_proc.h"
#include "version.h"
#include "drive.h"

/*
ISR(BADISR_vect){
}
*/

static inline void init(void)
{
	power_all_disable();

	usart_init();
	adc_init();
	motors_init();
	sei();
  
	fputs_P(version_str,stdout);
}

static uint16_t adc_vals[ADC_CT];

static struct sensor_s sensors[ADC_CT];
static struct line_s line = LINE_INIT(sensors);
static struct pid_s pid_turn;

static int16_t motor_velocity = MOTOR_SPEED_MAX;

__attribute__((noreturn))
void main(void)
{
	init();
	for(;;) {
		if (usart_new_msg()) {
			process_msg();
		}

		adc_val_cpy(adc_vals);
		int16_t pos = line_update(&line, 1, adc_vals);
		int16_t turn = pid_update(&pid_turn, 1, pos);
		/* XXX: motor speed should be throtled in some cases */
		drive_set(motor_velocity, turn);
	}
}

