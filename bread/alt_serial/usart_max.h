#ifndef _SERIAL_PORTS_H
#define _SERIAL_PORTS_H

#define USART_BAUDRATE 19200 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define FALSE	0
#define TRUE	1

#define TX_QUEUE_SIZE	32
#define TX_QUEUE_INDEX_MASK	TX_QUEUE_SIZE-1

void Serial_Port_Init(void);
void Serial_Port_Write(unsigned char byte);

#endif
