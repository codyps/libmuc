#include <avr/io.h>
#include <util/delay.h>
#include "common.h"

#define DELAY_TIME 250
#define LED_PIN PB5
#define LED_P B

void led_init(void)
{
	DDR(LED_P) |= (1 << LED_PIN);
}

void led_flash(uint8_t i)
{
	do {
		PORT(LED_P) |= (1 << LED_PIN);
		_delay_ms(DELAY_TIME);
		PORT(LED_P) &= ~(1 << LED_PIN);
		_delay_ms(DELAY_TIME);
	} while(--i);
	_delay_ms(DELAY_TIME);
}
