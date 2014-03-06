#ifndef USART_REG_COMPAT_H_
#define USART_REG_COMPAT_H_

/* Provide REGNAME0 in addition to REGNAME for some chips. */
#ifndef UCSR0A
#warn "enabling compatability USART0 defintion"

#ifdef  power_usart_enable
#define power_usart0_enable() power_usart_enable()
#else
#define power_usart0_enable()
#endif

/* note the RX vs RXC here */
#define USART0_RX_vect		USART_RXC_vect
#define USART0_UDRE_vect	USART_UDRE_vect

#define UCSR0A UCSRA
#define UCSR0B UCSRB
#define UCSR0C UCSRC
#define UDR0 UDR

/* UCSRA */
#define RXC0	RXC
#define TXC0	TXC
#define UDRE0	UDRE
#define FE0	FE
#define DOR0	DOR
#define PE0	PE
#define U2X0	U2X
#define MPCM0	MPCM

/* UCSRB */
#define RXCIE0	RXCIE
#define TXCIE0	TXCIE
#define UDRIE0	UDRIE
#define RXEN0	RXEN
#define	TXEN0	TXEN
#define UCSZ02	UCSZ2
#define RXB80	RXB8
#define TXB80	TXB8

/* UCSRC */
#define URSEL0	URSEL
#define UMSEL0	UMSEL
#define UPM01	UPM1
#define UPM00	UPM0
#define USBS0	USBS
#define UCSZ01	UCSZ1
#define UCSZ00	UCSZ0
#define UCPOL0	UCPOL

#endif /* defined (UCSR0A) */

#define REGN_A(s, n) CAT2(s, n)

/* Use SET_UBRR() for UBRR */
#ifdef UBRR0
# define SET_UBRR(num, val) do { REGN_A(UBRR, num) = val; } while (0)
#else
# define SET_UBRR(num, val) do { UBRRH = val >> 8; UBRRL = val & 0xff; } while (0)
#endif

#endif

