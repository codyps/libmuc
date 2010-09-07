#include <avr/io.h>

#define START_SENTINAL 0x7e
#define ESC_BYTE 0x7D
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

	uint8_t data = UDR0;
	if (data == START_SENTINAL) {
		/* prepare for start, reset packet position, etc. */
	} else if (is_escaped) {
		/* we previously recieved an escape char, transform data */
	} else if (data == ESC_BYTE) {
		is_escaped = 1;
	} else {
		/* normal reception */
	}
}


ISR(USART0_TX_vect)
{
	static bool is_escaped;
	/* Check the next byte in the queue */

	/* if it needs escaping, send the escape char */


}

__attribute__((noreturn))
void main(void)
{
	for(;;) {


	}
}
