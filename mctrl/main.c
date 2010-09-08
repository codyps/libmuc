#include <avr/io.h>

#define START_BYTE 0x7e
#define ESC_BYTE 0x7d
#define ESC_MASK 0x20
/* 0x7e => 0x7d, 0x5e
 * 0x7d => 0x7d, 0x5d
 */

/* [flag] [address] [ctrl] [data] [crc] [flag]
 * addr8 : [multicast:1] [node addr:6] ['1':1]
 * addr16: [multicast:1] [node addr:6] ['0':1] [node addr:7] [end addr:1]...
 * ctrl  :
 */


ISR(USART0_RX_vect)
{
	static bool is_escaped;
	uint8_t status = UCSR0A;
	uint8_t data = UDR0;

	/* check `status` for error conditions */
	if (status & (1 << FE0)) {
		/* Frame error */
		return;
	}

	if (status & (1 << DOR0)) {
		/* data over run */
		return;
	}

	if (status & (1 << UPE0)) {
		/* parity error */
		return;
	}

	if (data == START_BYTE) {
		/* TODO: prepare for start, reset packet position, etc. */
		return;
	}

	if (data == ESC_BYTE) {
		/* Possible error check: is_escaped should not already be true */
		is_escaped = true;
		return;
	}

	if (is_escaped) {
		/* Possible error check: is data of one of the allowed escaped bytes? */
		/* we previously recieved an escape char, transform data */
		data |= ESC_MASK;
	}

	/* normal reception */
	STORE(data);
}

ISR(USART0_TX_vect)
{
	static bool is_escaped;
	/* Check the next byte in the queue */
	uint8_t data = GET();


	if (is_escaped) {
		is_escaped = false;
		UDR0 = data & ~(ESC_MASK);
		return;
	}

	if (data == START_BYTE || data == ESC_BYTE) {
		is_escaped = true;
		UDR0 = ESC_BYTE;
		return;
	}

	UDR0 = data;
}

void uart0_init(void)
{
	/* Asyncronous 38400 */

	/* 1 start bit
	 * 8 data bits
	 * odd parity
	 * 1 stop bit
	 */
}

__attribute__((noreturn))
void main(void)
{
	for(;;) {


	}
}
