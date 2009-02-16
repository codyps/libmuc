/*! \file uart2.c \brief Dual UART driver with buffer support. */
//*****************************************************************************
//
// File Name	: 'uart2.h' changed to 'uart4.h' (see footnote *)
// Title		: Dual UART driver with buffer support
// Author		: Pascal Stang - Copyright (C) 2000-2004
// Created		: 11/20/2000
// Revised		: 07/04/2004
// Version		: 1.0
// Target MCU	: ATMEL AVR Series
// Editor Tabs	: 4
//
// Description	: This is a UART driver for AVR-series processors with two
//		hardware UARTs such as the mega161 and mega128 
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
// 
// 
//   * Code modified by societyofrobots.com to handle four UARTs - using ATmega2560
//	  Dec 6th, 2007
//
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include "buffer.h"
#include "uart4.h"

// UART global variables
// flag variables
volatile u08   uartReadyTx[4];
volatile u08   uartBufferedTx[4];
// receive and transmit buffers
cBuffer uartRxBuffer[4];
cBuffer uartTxBuffer[4];
unsigned short uartRxOverflow[4];
#ifndef UART_BUFFER_EXTERNAL_RAM
	// using internal ram,
	// automatically allocate space in ram for each buffer
	static char uart0RxData[UART0_RX_BUFFER_SIZE];
	static char uart0TxData[UART0_TX_BUFFER_SIZE];
	static char uart1RxData[UART1_RX_BUFFER_SIZE];
	static char uart1TxData[UART1_TX_BUFFER_SIZE];
	static char uart2RxData[UART2_RX_BUFFER_SIZE];
	static char uart2TxData[UART2_TX_BUFFER_SIZE];
	static char uart3RxData[UART3_RX_BUFFER_SIZE];
	static char uart3TxData[UART3_TX_BUFFER_SIZE];
#endif

typedef void (*voidFuncPtru08)(unsigned char);
volatile static voidFuncPtru08 UartRxFunc[4];

void uartInit(void)
{
	// initialize all uarts
	uart0Init();
	uart1Init();
	uart2Init();
	uart3Init();
}

void uart0Init(void)
{
	// initialize the buffers
	uart0InitBuffers();
	// initialize user receive handlers
	UartRxFunc[0] = 0;
	// enable RxD/TxD and interrupts
	outb(UCSR0B, BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN));
	// set default baud rate
	uartSetBaudRate(0, UART0_DEFAULT_BAUD_RATE); 
	// initialize states
	uartReadyTx[0] = TRUE;
	uartBufferedTx[0] = FALSE;
	// clear overflow count
	uartRxOverflow[0] = 0;
	// enable interrupts
	sei();
}

void uart1Init(void)
{
	// initialize the buffers
	uart1InitBuffers();
	// initialize user receive handlers
	UartRxFunc[1] = 0;
	// enable RxD/TxD and interrupts
	outb(UCSR1B, BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN));
	// set default baud rate
	uartSetBaudRate(1, UART1_DEFAULT_BAUD_RATE);
	// initialize states
	uartReadyTx[1] = TRUE;
	uartBufferedTx[1] = FALSE;
	// clear overflow count
	uartRxOverflow[1] = 0;
	// enable interrupts
	sei();
}

void uart2Init(void)
{
	// initialize the buffers
	uart2InitBuffers();
	// initialize user receive handlers
	UartRxFunc[2] = 0;
	// enable RxD/TxD and interrupts
	outb(UCSR2B, BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN));
	// set default baud rate
	uartSetBaudRate(2, UART2_DEFAULT_BAUD_RATE);
	// initialize states
	uartReadyTx[2] = TRUE;
	uartBufferedTx[2] = FALSE;
	// clear overflow count
	uartRxOverflow[2] = 0;
	// enable interrupts
	sei();
}

void uart3Init(void)
{
	// initialize the buffers
	uart3InitBuffers();
	// initialize user receive handlers
	UartRxFunc[3] = 0;
	// enable RxD/TxD and interrupts
	outb(UCSR3B, BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN));
	// set default baud rate
	uartSetBaudRate(3, UART3_DEFAULT_BAUD_RATE);
	// initialize states
	uartReadyTx[3] = TRUE;
	uartBufferedTx[3] = FALSE;
	// clear overflow count
	uartRxOverflow[3] = 0;
	// enable interrupts
	sei();
}

void uart0InitBuffers(void)
{
	#ifndef UART_BUFFER_EXTERNAL_RAM
		// initialize the UART0 buffers
		bufferInit(&uartRxBuffer[0], (u08*) uart0RxData, UART0_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[0], (u08*) uart0TxData, UART0_TX_BUFFER_SIZE);
	#else
		// initialize the UART0 buffers
		bufferInit(&uartRxBuffer[0], (u08*) UART0_RX_BUFFER_ADDR, UART0_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[0], (u08*) UART0_TX_BUFFER_ADDR, UART0_TX_BUFFER_SIZE);
	#endif
}

void uart1InitBuffers(void)
{
	#ifndef UART_BUFFER_EXTERNAL_RAM
		// initialize the UART1 buffers
		bufferInit(&uartRxBuffer[1], (u08*) uart1RxData, UART1_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[1], (u08*) uart1TxData, UART1_TX_BUFFER_SIZE);
	#else
		// initialize the UART1 buffers
		bufferInit(&uartRxBuffer[1], (u08*) UART1_RX_BUFFER_ADDR, UART1_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[1], (u08*) UART1_TX_BUFFER_ADDR, UART1_TX_BUFFER_SIZE);
	#endif
}

void uart2InitBuffers(void)
{
	#ifndef UART_BUFFER_EXTERNAL_RAM
		// initialize the UART2 buffers
		bufferInit(&uartRxBuffer[2], (u08*) uart2RxData, UART2_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[2], (u08*) uart2TxData, UART2_TX_BUFFER_SIZE);
	#else
		// initialize the UART2 buffers
		bufferInit(&uartRxBuffer[2], (u08*) UART2_RX_BUFFER_ADDR, UART2_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[2], (u08*) UART2_TX_BUFFER_ADDR, UART2_TX_BUFFER_SIZE);
	#endif
}

void uart3InitBuffers(void)
{
	#ifndef UART_BUFFER_EXTERNAL_RAM
		// initialize the UART3 buffers
		bufferInit(&uartRxBuffer[3], (u08*) uart3RxData, UART3_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[3], (u08*) uart3TxData, UART3_TX_BUFFER_SIZE);
	#else
		// initialize the UART3 buffers
		bufferInit(&uartRxBuffer[3], (u08*) UART3_RX_BUFFER_ADDR, UART3_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[3], (u08*) UART3_TX_BUFFER_ADDR, UART3_TX_BUFFER_SIZE);
	#endif
}

void uartSetRxHandler(u08 nUart, void (*rx_func)(unsigned char c))
{
	// make sure the uart number is within bounds
	if(nUart < 4)
	{
		// set the receive interrupt to run the supplied user function
		UartRxFunc[nUart] = rx_func;
	}
}

void uartSetBaudRate(u08 nUart, u32 baudrate)
{
	// calculate division factor for requested baud rate, and set it
	u16 bauddiv = ((F_CPU+(baudrate*8L))/(baudrate*16L)-1);
	if(nUart==3)
	{
		outb(UBRR3L, bauddiv);
		#ifdef UBRR3H
		outb(UBRR3H, bauddiv>>8);
		#endif
	}
	else if(nUart==2)
	{
		outb(UBRR2L, bauddiv);
		#ifdef UBRR2H
		outb(UBRR2H, bauddiv>>8);
		#endif
	}
	else if(nUart==1)
	{
		outb(UBRR1L, bauddiv);
		#ifdef UBRR1H
		outb(UBRR1H, bauddiv>>8);
		#endif
	}
	else
	{
		outb(UBRR0L, bauddiv);
		#ifdef UBRR0H
		outb(UBRR0H, bauddiv>>8);
		#endif
	}
}

cBuffer* uartGetRxBuffer(u08 nUart)
{
	// return rx buffer pointer
	return &uartRxBuffer[nUart];
}

cBuffer* uartGetTxBuffer(u08 nUart)
{
	// return tx buffer pointer
	return &uartTxBuffer[nUart];
}

void uartSendByte(u08 nUart, u08 txData)
{
	// wait for the transmitter to be ready
//	while(!uartReadyTx[nUart]);
	// send byte
	if(nUart==3)
	{
		while(!(UCSR3A & (1<<UDRE3)));
		outb(UDR3, txData);
	}
	else if(nUart==2)
	{
		while(!(UCSR2A & (1<<UDRE2)));
		outb(UDR2, txData);
	}
	else if(nUart==1)
	{
		while(!(UCSR1A & (1<<UDRE1)));
		outb(UDR1, txData);
	}
	else
	{
		while(!(UCSR0A & (1<<UDRE0)));
		outb(UDR0, txData);
	}
	// set ready state to FALSE
	uartReadyTx[nUart] = FALSE;
}

void uart0SendByte(u08 data)
{
	// send byte on UART0
	uartSendByte(0, data);
}

void uart1SendByte(u08 data)
{
	// send byte on UART1
	uartSendByte(1, data);
}

void uart2SendByte(u08 data)
{
	// send byte on UART2
	uartSendByte(2, data);
}

void uart3SendByte(u08 data)
{
	// send byte on UART3
	uartSendByte(3, data);
}

int uart0GetByte(void)
{
	// get single byte from receive buffer (if available)
	u08 c;
	if(uartReceiveByte(0,&c))
		return c;
	else
		return -1;
}

int uart1GetByte(void)
{
	// get single byte from receive buffer (if available)
	u08 c;
	if(uartReceiveByte(1,&c))
		return c;
	else
		return -1;
}

int uart2GetByte(void)
{
	// get single byte from receive buffer (if available)
	u08 c;
	if(uartReceiveByte(2,&c))
		return c;
	else
		return -1;
}

int uart3GetByte(void)
{
	// get single byte from receive buffer (if available)
	u08 c;
	if(uartReceiveByte(3,&c))
		return c;
	else
		return -1;
}


u08 uartReceiveByte(u08 nUart, u08* rxData)
{
	// make sure we have a receive buffer
	if(uartRxBuffer[nUart].size)
	{
		// make sure we have data
		if(uartRxBuffer[nUart].datalength)
		{
			// get byte from beginning of buffer
			*rxData = bufferGetFromFront(&uartRxBuffer[nUart]);
			return TRUE;
		}
		else
			return FALSE;			// no data
	}
	else
		return FALSE;				// no buffer
}

void uartFlushReceiveBuffer(u08 nUart)
{
	// flush all data from receive buffer
	bufferFlush(&uartRxBuffer[nUart]);
}

u08 uartReceiveBufferIsEmpty(u08 nUart)
{
	return (uartRxBuffer[nUart].datalength == 0);
}

void uartAddToTxBuffer(u08 nUart, u08 data)
{
	// add data byte to the end of the tx buffer
	bufferAddToEnd(&uartTxBuffer[nUart], data);
}

void uart0AddToTxBuffer(u08 data)
{
	uartAddToTxBuffer(0,data);
}

void uart1AddToTxBuffer(u08 data)
{
	uartAddToTxBuffer(1,data);
}

void uart2AddToTxBuffer(u08 data)
{
	uartAddToTxBuffer(2,data);
}

void uart3AddToTxBuffer(u08 data)
{
	uartAddToTxBuffer(3,data);
}

void uartSendTxBuffer(u08 nUart)
{
	// turn on buffered transmit
	uartBufferedTx[nUart] = TRUE;
	// send the first byte to get things going by interrupts
	uartSendByte(nUart, bufferGetFromFront(&uartTxBuffer[nUart]));
}

u08 uartSendBuffer(u08 nUart, char *buffer, u16 nBytes)
{
	register u08 first;
	register u16 i;

	// check if there's space (and that we have any bytes to send at all)
	if((uartTxBuffer[nUart].datalength + nBytes < uartTxBuffer[nUart].size) && nBytes)
	{
		// grab first character
		first = *buffer++;
		// copy user buffer to uart transmit buffer
		for(i = 0; i < nBytes-1; i++)
		{
			// put data bytes at end of buffer
			bufferAddToEnd(&uartTxBuffer[nUart], *buffer++);
		}

		// send the first byte to get things going by interrupts
		uartBufferedTx[nUart] = TRUE;
		uartSendByte(nUart, first);
		// return success
		return TRUE;
	}
	else
	{
		// return failure
		return FALSE;
	}
}

// UART Transmit Complete Interrupt Function
void uartTransmitService(u08 nUart)
{
	// check if buffered tx is enabled
	if(uartBufferedTx[nUart])
	{
		// check if there's data left in the buffer
		if(uartTxBuffer[nUart].datalength)
		{
			// send byte from top of buffer
			if(nUart==3)
				outb(UDR3,  bufferGetFromFront(&uartTxBuffer[3]) );
			else if(nUart==2)
				outb(UDR2,  bufferGetFromFront(&uartTxBuffer[2]) );
			else if(nUart==1)
				outb(UDR1,  bufferGetFromFront(&uartTxBuffer[1]) );
			else
				outb(UDR0,  bufferGetFromFront(&uartTxBuffer[0]) );
		}
		else
		{
			// no data left
			uartBufferedTx[nUart] = FALSE;
			// return to ready state
			uartReadyTx[nUart] = TRUE;
		}
	}
	else
	{
		// we're using single-byte tx mode
		// indicate transmit complete, back to ready
		uartReadyTx[nUart] = TRUE;
	}
}

// UART Receive Complete Interrupt Function
void uartReceiveService(u08 nUart)
{
	u08 c;
	// get received char
	if(nUart==3)
		c = inb(UDR3);
	else if(nUart==2)
		c = inb(UDR2);
	else if(nUart==1)
		c = inb(UDR1);
	else
		c = inb(UDR0);

	// if there's a user function to handle this receive event
	if(UartRxFunc[nUart])
	{
		// call it and pass the received data
		UartRxFunc[nUart](c);
	}
	else
	{
		// otherwise do default processing
		// put received char in buffer
		// check if there's space
		if( !bufferAddToEnd(&uartRxBuffer[nUart], c) )
		{
			// no space in buffer
			// count overflow
			uartRxOverflow[nUart]++;
		}
	}
}

	// service UART transmit interrupt
UART_INTERRUPT_HANDLER(SIG_USART0_TRANS)      
{
	uartTransmitService(0);
}

UART_INTERRUPT_HANDLER(SIG_USART1_TRANS)      
{
	uartTransmitService(1);
}

UART_INTERRUPT_HANDLER(SIG_USART2_TRANS)      
{
	uartTransmitService(2);
}

UART_INTERRUPT_HANDLER(SIG_USART3_TRANS)      
{
	uartTransmitService(3);
}

	// service UART receive interrupt
UART_INTERRUPT_HANDLER(SIG_USART0_RECV)      
{
	uartReceiveService(0);
}

UART_INTERRUPT_HANDLER(SIG_USART1_RECV)      
{
	uartReceiveService(1);
}

UART_INTERRUPT_HANDLER(SIG_USART2_RECV)      
{
	uartReceiveService(2);
}

UART_INTERRUPT_HANDLER(SIG_USART3_RECV)      
{
	uartReceiveService(3);
}
