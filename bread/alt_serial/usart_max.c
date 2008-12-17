#include "defines.h"
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include "usart_max.h"

volatile unsigned char Tx_Queue[TX_QUEUE_SIZE];
volatile unsigned char Tx_Queue_Full = FALSE;
volatile unsigned char Tx_Queue_Empty = TRUE;
volatile unsigned char Tx_Queue_Read_Index = 0;
unsigned char Tx_Queue_Write_Index = 0;
volatile unsigned char Tx_Queue_Byte_Count = 0;

static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void Serial_Port_Init(void)
{
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);	// Enable TX and RX
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);	// 8-bit, asyncronous

	UBRR0L = BAUD_PRESCALE; 		// Load lower 8-bits of the baud rate value into the low byte of the UBRR register 
	UBRR0H = (BAUD_PRESCALE >> 8); 	// Load upper 8-bits of the baud rate value into the high byte of the UBRR register 

	UCSR0B |= (1 << UDRE0); 		// Enable the USART Recieve Complete interrupt (USART_RXC)
	
	stdout = &mystdout; 			// Required for printf init
}

void Serial_Port_Write(unsigned char byte)
{
	// if the queue is full, wait here until space is available
	while (Tx_Queue_Full);
	
	// put the byte on the circular queue
	Tx_Queue[Tx_Queue_Write_Index] = byte;
	
	// increment the queue byte count
	Tx_Queue_Byte_Count++;
	
	// increment the write pointer
	Tx_Queue_Write_Index++;
	
	// if the index pointer overflowed, cut off the high order bit
	Tx_Queue_Write_Index &= TX_QUEUE_INDEX_MASK;
	
	// disable interrupt
	UCSR0B &= (uint8_t)~(1 << UDRE0);
	
	// is the circular queue full?
	if (Tx_Queue_Read_Index == Tx_Queue_Write_Index)
	{
		Tx_Queue_Full = TRUE;
	}
	
	// enable the interrupt
	UCSR0B |= (1 << UDRE0);
	
	// we just added a byte to the queue, its not empty
	Tx_Queue_Empty = FALSE;
	
}

ISR(USART0_UDRE_vect) 
{ 
	// if the queue is empty turn off the interrupt
	if (Tx_Queue_Empty)
	{
		UCSR0B &= (uint8_t)~(1 << UDRE0);
	}
	else
	{
		// if there is something to write, write it
		UDR0 = Tx_Queue[Tx_Queue_Read_Index];
		
		// we just wrote a byte, decrease byte count
		Tx_Queue_Byte_Count--;
		
		Tx_Queue_Read_Index++;
		
		Tx_Queue_Read_Index &= TX_QUEUE_INDEX_MASK;
		
		if (Tx_Queue_Read_Index == Tx_Queue_Write_Index)
		{
			Tx_Queue_Empty = TRUE;
		}
		
		// queue can't be full, we just wrote a byte
		Tx_Queue_Full = FALSE;
	}
}

static int uart_putchar(char c, FILE *stream)
{
    if (c == '\n') uart_putchar('\r', stream);
  
    Serial_Port_Write(c);
    
    return 0;
}
