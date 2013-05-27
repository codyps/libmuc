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

#include <penny/math.h>
#include <penny/circ_buf.h>
#include <muc/usart_reg_compat.h>

#define usart_var(n, name) usart##n##_##name
#define usart_fn(n, name) usart##n##_##name

#define BAUD_TO_UBRR_1X(f_cpu, baud) (((f_cpu) + 8UL * (baud)) / (16UL * (baud)) -1UL)
#define BAUD_TO_UBRR_2X(f_cpu, baud) (((f_cpu) + 4UL * (baud)) / ( 8UL * (baud)) -1UL)

#define BAUD_TO_UBRR(f_cpu, baud)	\
	(USART_USE_2X(f_cpu, baud) ? BAUD_TO_UBRR_2X(f_cpu, baud) : BAUD_TO_UBRR_1X(f_cpu, baud))

#define UBRR_TO_BAUD_1X(f_cpu, ubrr) ((f_cpu)/(16 * ((ubrr) + 1)))
#define UBRR_TO_BAUD_2X(f_cpu, ubrr) ((f_cpu)/(8 * ((ubrr) + 1)))

#define UBRR_TO_BAUD(f_cpu, baud)	\
	(USART_USE_2X(f_cpu, baud) ? UBRR_TO_BAUD_2X(f_cpu, baud) : UBRR_TO_BAUD_1X(f_cpu, baud))

#define USART_USE_2X(f_cpu, baud) (UBRR_ERROR_X(2, f_cpu, baud) < UBRR_ERROR_X(1, f_cpu, baud))

#define UBRR_ERROR_X(x, f_cpu, baud) \
	ABS(UBRR_TO_BAUD_##x##X(f_cpu, BAUD_TO_UBRR_##x##X(f_cpu, baud)) - baud)

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


#define DEFINE_USART_IMPL_TRANSMIT_(num, tq_sz)\
	static struct U_t(num) {					\
		uint8_t head;						\
		uint8_t tail;						\
		char buf[tq_sz];					\
	} U_t(num);							\
	static inline void usart_fn(num, udrei_on)(void)		\
	{								\
		UCSR##num##B |= (1 << UDRIE##num);			\
	}								\
	static inline void usart_fn(num, udrei_off)(void)		\
	{								\
		UCSR##num##B &= ~(1 << UDRIE##num);	\
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
	static void usart_fn(num, flush_tx)(void)			\
	{								\
		U_t(num).tail = U_t(num).head;				\
	}								\
	ISR(USART##num##_UDRE_vect)					\
	{								\
		if (circ_empty(U_t(num))) {				\
			usart_fn(num, udrei_off)();			\
			return;						\
		}							\
		UDR##num = U_t(num).buf[U_t(num).tail];			\
		U_t(num).tail = CIRC_NEXT(U_t(num).tail,		\
					sizeof(U_t(num).buf));		\
	}

#define DEFINE_USART_IMPL_RECV_(num, rq_sz)				\
	static struct U_r(num) {					\
		uint8_t head;						\
		uint8_t tail;						\
		char buf[rq_sz];					\
	} U_r(num);							\
	static uint8_t usart_var(num, msg);				\
									\
	static inline void usart_fn(num, rxi_on)(void)			\
	{								\
		UCSR##num##B |= (1 << RXCIE##num);			\
	}								\
	static inline void usart_fn(num, rxi_off)(void)			\
	{								\
		UCSR##num##B &= ~(1 << RXCIE##num);			\
	}								\
	static void usart_fn(num, flush_rx)(void)			\
	{								\
		U_r(num).tail = U_r(num).head;				\
	}								\
									\
	static void usart_fn(num, flush_rx_to)(char end_delim)		\
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
	static bool usart_fn(num, new_msg)(void)			\
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
	static bool usart_fn(num, hasc)(void)				\
	{								\
		return U_r(num).tail != U_r(num).head;			\
	}								\
	static char usart_fn(num, getc)(void)				\
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
									\
	ISR(USART##num##_RX_vect)					\
	{								\
		uint8_t next_head = CIRC_NEXT(U_r(num).head,		\
					sizeof(U_r(num).buf));		\
		if (U_r(num).tail == next_head)				\
			return;						\
		U_r(num).buf[U_r(num).head] = UDR##num;			\
		if (U_r(num).buf[U_r(num).head] == msg_delim)		\
			usart_var(num, msg) ++;				\
		U_r(num).head = next_head;				\
	}

#define DEFINE_USART_IMPL_TRANSMIT(num, usart_baud, tq_sz)		\
	DEFINE_USART_IMPL_TRANSMIT_(num, tq_sz);			\
	static FILE usart_var(num, io) = FDEV_SETUP_STREAM(		\
			  usart_fn(num, putchar)			\
			, NULL						\
			, _FDEV_SETUP_WRITE);				\
	static FILE *usart_fn(num, init)(void)				\
	{								\
		power_usart##num##_enable();				\
		UCSR##num##B = 0;					\
		UCSR##num##A = USART_USE_2X(F_CPU, usart_baud)		\
			? (1 << U2X##num)				\
			: 0;						\
		SET_UBRR(BAUD_TO_UBRR(F_CPU, usart_baud));		\
		UCSR##num##C = (1 << UCSZ##num##0) |			\
				(1 << UCSZ##num##1) |			\
				(1 << URSEL##num);			\
		UCSR##num##B = (1 << TXEN##num);			\
		return &usart_var(num, io);				\
	}								\

/* Uses the define "F_CPU" to handle computions */
#define DEFINE_USART_IMPL(num, usart_baud, tq_sz, rq_sz, msg_delim)	\
	DEFINE_USART_IMPL_TRANSMIT_(num, tq_sz)				\
	DEFINE_USART_IMPL_RECV_(num, rq_sz)				\
	static FILE usart_var(num, io) = FDEV_SETUP_STREAM(		\
			  usart_fn(num, putchar)			\
			, usart_fn(num, getchar)			\
			, _FDEV_SETUP_RW);				\
									\
	static FILE *usart_fn(num, init)(void)				\
	{								\
		power_usart##num##_enable();				\
		UCSR##num##B = 0;					\
		UCSR##num##A = USART_USE_2X(F_CPU, usart_baud)		\
			? (1 << U2X##num)				\
			: 0;						\
		SET_UBRR(BAUD_TO_UBRR(F_CPU, usart_baud));		\
		UCSR##num##C = (1 << UCSZ##num##0) |			\
				(1 << UCSZ##num##1) |			\
				(1 << URSEL##num);			\
		UCSR##num##B = (1 << TXEN##num) |			\
				(1 << RXEN##num) |			\
				(1 << RXCIE##num);			\
		return &usart_var(num, io);				\
	}								\

#endif
