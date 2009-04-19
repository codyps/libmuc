/*
	Axon Code for I2C testing
 */

#include "global.h"

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart4.h"
#include "i2c.h"
#include "rprintf.h"

#include "i2c_HMC6343_cmd.h"

static int usart1_putchar(char c, FILE *stream) {
	uart1SendByte(c);
	return 0;
}

static FILE usart1_stdout = FDEV_SETUP_STREAM(usart1_putchar, NULL ,_FDEV_SETUP_WRITE);



void init(void) {

	uartInit();
	rprintfInit(uart1AddToTxBuffer);
	i2cInit();
	stdout = &usart1_stdout;

	sei();
	rprintf("\n\n[main]: init done\n\n");	
}

int main(void) { 	
	uint8_t cmd[1] = { HMC6343_POST_HEAD };
	uint8_t raw_data[6];
	struct heading_data_t data;

	init();
	for (;;) {
		i2cMasterSend(HMC6343_ADDR, 1, cmd);
		i2cMasterReceive(HMC6343_ADDR, 6, raw_data);
		data.head_msb  = raw_data[0];
		data.head_lsb  = raw_data[1];
		data.pitch_msb = raw_data[2];
		data.pitch_lsb = raw_data[3];
		data.roll_msb  = raw_data[4];
		data.roll_lsb  = raw_data[5];
		printf("head: %u ", data.head);
		printf("pitch: %d ", data.pitch);
		printf("roll: %d\n", data.roll);
		uartSendTxBuffer(1);
		_delay_ms(5000);
	}
	return 0;
}

