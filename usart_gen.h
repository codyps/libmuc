#ifndef USART_GEN_H_
#define USART_GEN_H_ 1

#include <stdbool.h>
#include <stdint.h>
#include "common/circ_buf.h"

#define usart_var(n, name) usart##n##_##name
#define usart_fn(n, name) usart##n##_##name

#define UBRR_CALC(f_cpu, baud) (((f_cpu) + 8UL * (baud)) / (16UL * (baud)) -1UL)

/* short hand for the queue */
#define U_r(num) usart_var(num, rq)
#define U_t(num) usart_var(num, tq)

/* composition of register & bit names are handled by these macros. */
#define REGN_A(base, n) CAT2(base, n)
#define REGN_I(base, n, x) CAT3(base, n, x)

#define CAT2(a, b) a##b
#define CAT3(a, b, c) a##b##c

#define DEFINE_USART_DEF(num)						\
	FILE *usart_fn(num, init)(void);				\
	/* non-blocking input mechanisms */				\
	bool usart_fn(num, hasc)(void);					\
	char usart_fn(num, getc)(void);					\
	/* flushes */							\
	void usart_fn(num, flush_rx_to)(char end_delim);		\
	void usart_fn(num, flush_rx)(void);				\
	void usart_fn(num, flush_tx)(void);				\
	/* indicates a '\n' has been recieved. TODO: generalize	*/	\
	bool usart_fn(num, new_msg)(void);


#define DEFINE_USART_IMPL(num, usart_baud, tq_sz, rq_sz)		\
	static struct U_t(num) {					\
		uint8_t head;						\
		uint8_t tail;						\
		char buf[tq_sz];					\
	} U_t(num)							\
	static struct U_r(num) {					\
		uint8_t head;						\
		uint8_t tail;						\
		char buf[rq_sz];					\
	} U_r(num);							\
	static uint8_t usart_var(num, msg)				\
									\
	static int usart_fn(num, getchar)(FILE *stream);		\
	static int usart_fn(num, putchar)(char c, FILE *stream);	\
	static FILE usart_var(num, io) = FDEV_SETUP_STREAM(		\
			  usart_fn(num, putchar)			\
			, usart_fn(num, getchar)			\
			, _FDEV_SETUP_RW);				\
									\
	static inline void usart_fn(num, udrei_on)(void)		\
	{								\
		REGN_I(UCSR, num, B) |= (1 << REGN_A(UDRIE, num));	\
	}								\
	static inline void usart_fn(num, udrei_off)(void)		\
	{								\
		REGN_I(UCSR, num, B) &= ~(1 << REGN_A(UDRIE, num));	\
	}								\
	static inline void usart_fn(num, rxi_on)(void)			\
	{								\
		REGN_I(UCSR, num, B) |= (1 << REGN_A(RXCIE, num));	\
	}								\
	static inline void usart_fn(num, rxi_off)(void)			\
	{								\
		REGN_I(UCSR, num, B) &= ~(1 << REGN_A(RECIE, num));	\
	}								\
									\
	void usart_fn(num, flush_rx)(void)				\
	{								\
		U_r(num).tail = U_r(num).head;				\
	}								\
	void usart_fn(num, flush_tx)(void)				\
	{								\
		U_t(num).tail = U_t(num).head;				\
	}								\
	void usart_fn(num, flush_rx_to)(char end_delim)			\
	{								\
		while(U_r(num).tail !=	U_r(num).head) {		\
			uint8_t d = U_r(num).buf[U_r(num).tail];	\
			if (d == end_delim)				\
				return;					\
			U_r(num).tail = CIRC_NEXT(U_r(num).tail,	\
						sizeof(U_r(num).buf));	\
		}							\
	}								\
									\
	bool usart_fn(num, new_msg)(void)				\
	{								\
		bool ret = false;					\
		usart_fn(num, rxi_off)();				\
		if (usart_var(num, msg)) {				\
			usart_var(num, msg) --;				\
			ret = true;					\
		}							\
		usart_fn(num, rxi_on)();				\
		return ret;						\
	}								\
									\
	bool usart_fn(num, hasc)(void)					\
	{								\
		return U_r(num).tail != U_r(num).head;			\
	}								\
	char usart_fn(num, getc)(void)					\
	{								\
		char ret = U_r(num).buf[U_r(num).tail];			\
		U_r(num).tail = CIRC_NEXT(U_r(num).tail,		\
					sizeof(U_r(num).buf));		\
		return ret;						\
	}								\
									\
	static int usart_fn(num, getchar)(FILE *stream)			\
	{								\
		uint8_t cur_tail = U_r(num).tail;			\
		while (cur_tail == U_r(num).head) {			\
			barrier();					\
		}							\
		char ret = U_r(num).buf[cur_tail];			\
		U_r(num).tail = CIRC_NEXT(cur_tail,			\
					sizeof(U_r(num).buf));		\
		return ret;						\
	}								\
	static int usart_fn(num, putchar)(char c, FILE *stream)		\
	{								\
		uint8_t next_head = CIRC_NEXT(U_t(num).head,		\
					sizeof(U_t(num).buf));		\
		while(U_t(num).tail == next_head) {			\
			barrier();					\
		}							\
		U_t(num).buf[next_head] = c;				\
		U_t(num).head = next_head;				\
		return 0;						\
	}								\
									\
	FILE *usart_fn(num, init)(void)					\
	{								\
		CAT3(power_usart, num, _enable)();			\
		REGN_I(UCSR, num, B) = 0;				\
		REGN_I(UCSR, num, A) = 0;				\
		REGN_A(UBRR, num) = UBRR_CALC(F_CPU, usart_baud);	\
		REGN_I(UCSR, num, C) = (1 << REGN_I(UCSZ,num,0)) |	\
				(1 << REGN_I(UCSZ, num, 1));		\
		REGN_I(UCSR, num, B) = (1 << REGN_A(TXEN, num)) |	\
				(1 << REGN_A(RXEN, num)) |		\
				(1 << REGN_A(RXCIE, num));		\
		return &usart_var(num, io);				\
	}								\
									\
	ISR(CAT3(USART, num, _UDRE_vect))				\
	{								\
		if (U_t(num).head == U_t(num).tail) {			\
			usart_udrei_off();				\
			return;						\
		}							\
		REGN_A(UDR, num) = U_t(num).buf[U_t(num).tail];		\
		U_t(num).tail = CIRC_NEXT(U_t(num).tail,		\
					sizeof(U_t(num).buf));		\
	}								\
	ISR(CAT3(USART, num, _RX_vect))					\
	{								\
		uint8_t next_head = CIRC_NEXT(U_r(num).head,		\
					sizeof(U_r(num).buf));		\
		if (U_r(num).tail == next_head) {			\
			return;						\
		}							\
		U_r(num).buf[U_r(num).head] = REGN_A(UDR, num);		\
		if (U_r(num).buf[U_r(num).head] == msg_delim)		\
			usart_var(num, msg) ++;				\
		U_r(num).head = next_head;				\
	}

#endif
