#define USART_NUMBER	0

#define U_A(_A)	COMB2(_A, USART_NUMBER )
#define U_I(_A,_B) COMB3(_A, USART_NUMBER ,_B)

#define COMB3(_A,_num,_C) _CAT3(_A, _num, _C)
#define _CAT3(a,b,c) a##b##c
#define COMB2(_A,_num) _CAT2(_A, _num)
#define _CAT2(a,b) a##b

// Defined:
// Functions
/*

#define usart_init U_I(usart,_init)
#define usart_flush_rx U_I(usart,_flush_rx)
#define usart_flush_tx U_I(usart,_flush_tx)



*/

// Used:
// Registers
//  Bits
#define UCSRA	U_I(UCSR,A)
 #define U2X	U_A(U2X)
 #define DOR    U_A(DOR)
 #define FE     U_A(FE)
 #define UDRE	U_A(UDRE)
 #define RXC	U_A(RXC)
#define UCSRB	U_I(UCSR,B)
 #define TXEN	U_A(TXEN)
 #define RXEN	U_A(RXEN)
 #define RXCIE	U_A(RXCIE)
 #define UDRIE	U_A(UDRIE)
#define UCSRC	U_I(UCSR,C)
 #define UCSZ0	U_I(UCSZ,0)
 #define UCSZ1	U_I(UCSZ,1)

#define UDR	U_A(UDR) 
#define UBRR	U_A(UBRR)

// Interrupt vectors
#ifndef USART_UDRE_vect
#define USART_UDRE_vect U_I(USART,_UDRE_vect)
#endif
#ifndef USART_RX_vect
#define USART_RX_vect	U_I(USART,_RX_vect)
#endif
// Functions
#define power_usart_enable U_I(power_usart,_enable)
