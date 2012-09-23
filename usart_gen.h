/*    libmuc / muc - a macro library for microcontrollers
 *    Copyright (C) 2012  Cody P Schafer <jmesmon@gmail.com>
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    See COPYING for a copy of the license.
 */
#ifndef USART_GEN_H_
#define USART_GEN_H_ 1

#include <stdbool.h>
#include <stdint.h>

#include <muc/muc.h>
#include <penny/circ_buf.h>

#define usart_var(n, name) usart##n##_##name
#define usart_fn(n, name) usart##n##_##name

#define BAUD_TO_UBRR(f_cpu, baud) (((f_cpu) + 8UL * (baud)) / (16UL * (baud)) -1UL)
#define BAUD_TO_UBRR_2X(f_cpu, baud)    (((f_cpu) + 4UL * (baud)) / (8UL * (baud)) -1UL)

#define UBRR_TO_BAUD(f_cpu, ubrr) ((f_cpu)/(16 * ((ubrr) + 1)))
#define UBRR_TO_BAUD_2X(f_cpu, ubrr) ((f_cpu)/(8 * ((ubrr) + 1)))

#define UBRR_ERROR(f_cpu, baud, ubrr)

/* short hand for the queue */
#define U_r(num) usart_var(num, rq)
#define U_t(num) usart_var(num, tq)

#define DEFINE_USART_DEF(num)						\
	/* FILE *usartX_init(void); */					\
	FILE *usart_fn(num, init)(void);				\
	/* bool usartX_hasc(void); */					\
	bool usart_fn(num, hasc)(void);					\
	/* bool usartX_getc(void); */					\
	char usart_fn(num, getc)(void);					\
	/* flushes */							\
	void usart_fn(num, flush_rx_to)(char end_delim);		\
	void usart_fn(num, flush_rx)(void);				\
	void usart_fn(num, flush_tx)(void);				\
	/* indicates a '\n' has been recieved. TODO: generalize	*/	\
	bool usart_fn(num, new_msg)(void);

/* Uses the define "F_CPU" to handle computions */
#define DEFINE_USART_IMPL(num, usart_baud, tq_sz, rq_sz, msg_delim)	\
	static struct U_t(num) {					\
		uint8_t head;						\
		uint8_t tail;						\
		char buf[tq_sz];					\
	} U_t(num);							\
	static struct U_r(num) {					\
		uint8_t head;						\
		uint8_t tail;						\
		char buf[rq_sz];					\
	} U_r(num);							\
	static uint8_t usart_var(num, msg);				\
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
		REGN_I(UCSR, num, B) &= ~(1 << REGN_A(RXCIE, num));	\
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
		while (cur_tail == ACCESS_ONCE(U_r(num).head))		\
			;						\
		char ret = U_r(num).buf[cur_tail];			\
		U_r(num).tail = CIRC_NEXT(cur_tail,			\
					sizeof(U_r(num).buf));		\
		return ret;						\
	}								\
	static int usart_fn(num, putchar)(char c, FILE *stream)		\
	{								\
		uint8_t head = U_t(num).head;				\
		uint8_t next_head = CIRC_NEXT(head, tq_sz);		\
		while(next_head == ACCESS_ONCE(U_t(num).tail))		\
			;						\
		U_t(num).buf[head] = c;					\
		U_t(num).head = next_head;				\
		usart_fn(num, udrei_on)();				\
		return 0;						\
	}								\
									\
	FILE *usart_fn(num, init)(void)					\
	{								\
		CAT3(power_usart, num, _enable)();			\
		REGN_I(UCSR, num, B) = 0;				\
		REGN_I(UCSR, num, A) = 0;				\
		REGN_A(UBRR, num) = BAUD_TO_UBRR(F_CPU, usart_baud);	\
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
		if (circ_empty(U_t(num))) {				\
			usart_fn(num, udrei_off)();			\
			return;						\
		}							\
		REGN_A(UDR, num) = U_t(num).buf[U_t(num).tail];		\
		U_t(num).tail = CIRC_NEXT(U_t(num).tail,		\
					sizeof(U_t(num).buf));		\
	}								\
									\
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
