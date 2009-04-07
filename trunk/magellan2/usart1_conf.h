//FIXME: Currently the following line does nothing, had to place the '5' directly in the S_# macros
#define USART_NUMBER	1

#define U_A(_A)	COMB2(_A, 1 )
#define U_I(_A,_B) COMB3(_A, 1 ,_B)

#define COMB3(_A,_num,_C) _A ## _num ## _C
#define COMB2(_A,_num) _A ## _num

//

#define UCSRA	U_I(UCSR,A)
 #define UDRE	U_A(UDRE)
 #define U2X	U_A(U2X)
#define UCSRB	U_I(UCSR,B)
 #define TXEN	U_A(TXEN)
 #define RXEN	U_A(RXEN)
 #define UDRIE	U_A(UDRIE)
#define UCSRC	U_I(UCSR,C)
 #define UCSZ0	U_I(UCSZ,0)
 #define UCSZ1	U_I(UCSZ,1)

#define UDR	U_A(UDR) 
#define UBRR	U_A(UBRR)

#define USART_UDRE_vect U_I(USART,_UDRE_vect)
