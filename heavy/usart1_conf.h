//FIXME: Currently the following line does nothing, had to place the '1' directly in the U_# macros
#define USART_NUMBER	1

#define EV(x) (x)

#define U_A(_A)	COMB2(_A, EV(USART_NUMBER) )
#define U_I(_A,_B) COMB3(_A, EV(USART_NUMBER) ,_B)

#define COMB3(_A,_num,_C) _A ## _num ## _C
#define COMB2(_A,_num) _A ## _num

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
 #define UDRE	U_A(UDRE)
 #define RXC	U_A(RXC)
 #define U2X	U_A(U2X)
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
#define USART_UDRE_vect U_I(USART,_UDRE_vect)
#define USART_RX_vect	U_I(USART,_RX_vect)
// Functions
#define power_usart_enable U_I(power_usart,_enable)
