#ifndef LIST_H_
#define LIST_H_

#include <stdint.h>
#include <stdbool.h>
#include "circ_buf.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#define __unused        __attribute__((unused))

#define CAT3(x,y,z) x##y##z

typedef int8_t list_error_t;

#define LIST_ERROR_NORET LIST_ERROR
#define LIST_ERROR(...) fprintf(stderr,"\nERROR: %s:%s():%d \n",__FILE__,__func__,__LINE__)

#define list_t(_name_) CAT3(list_,_name_,_t)

#define LIST_CT(list) CIRC_CNT((list).head,(list).tail,(list).size)
#define LIST_SZ(list) ((list).size)
#define LIST_PEAKF(list) ((list).buffer[((list).head - 1) & ((list).size - 1)])

/* tail = next loc to read from
 * head = next loc to write to
 * size = base 2 size of buffer */
#define _L_DEF_STRUCT(_name_,dattr,_data_t_,_index_t_) \
	typedef struct {                         \
		_data_t_ *const restrict buffer; \
		_index_t_ head;                  \
		_index_t_ tail;                  \
		const _index_t_ size;            \
	} dattr list_t(_name_);

#define LIST_INITIALIZER(buff) {             \
	.buffer = (buff),                    \
	.head = 0,                           \
	.tail = 0,                           \
	.size = sizeof(buff) / sizeof(*buff) \
	}

/* for a stack */
#define list_pop list_popf
#define list_push list_pushf

/* for a queue */
#define list_get list_popb
#define list_put list_pushf
#define list_remove list_popf

/* General operators. */
#define list_popf(_name_) CAT3(list_,_name_,_pop_front)
#define list_popb(_name_) CAT3(list_,_name_,_pop_back)

#define list_pushf(_name_)  CAT3(list_,_name_,_push_front)
#define list_pushb(_name_)  CAT3(list_,_name_,_push_back)
#define list_pushfo(_name_) CAT3(list_,_name_,_push_front_o)

#define list_peekb(_name_) CAT3(list_,_name_,_peek_back)
#define list_peekf(_name_) CAT3(list_,_name_,_peek_front)
#define list_peek(_name_)  CAT3(list_,_name_,_peek)

#define list_flush(_name_)   CAT3(list_,_name_,_flush)
#define list_full(_name_)    CAT3(list_,_name_,_full)
#define list_empty(_name_)   CAT3(list_,_name_,_empty)
#define list_valid_i(_name_) CAT3(list_,_name_,_valid_index)

/* Function generators */
#define _L_DEF_READ(_name_,_data_t_,_index_t_) \
__unused _index_t_ list_read(_name_)(list_t(_name_) *l, _data_t_ *buf, _index_t_ len) {\


#define _L_DEF_POPF(_name_,_data_t_)               \
__unused _data_t_ list_popf(_name_)(list_t(_name_) *l) {    \
	l->head = (l->head - 1) & (l->size - 1);   \
	_data_t_ head = l->buffer[ l->head ];      \
	return head;                               \
}

/* the (l->end >= l->sz) is a check for overflow. */
#define _L_DEF_POPB(_name_,_data_t_)                \
__unused _data_t_ list_popb(_name_)(list_t(_name_) *l) {     \
	_data_t_ last = l->buffer[l->tail];     \
	l->tail = (l->tail + 1) & (l->size - 1);\
	return last;                            \
}

#define _L_DEF_PUSHF(_name_,_data_t_)   \
__unused void list_pushf(_name_)(list_t(_name_) *l, _data_t_ x) { \
	l->buffer[ l->head ] = x;                \
	l->head = (l->head + 1) & (l->size - 1); \
}

#define _L_DEF_PUSHB(_name_,_data_t_) \
__unused void list_pushb(_name_)(list_t(_name_) *l, _data_t_ x) {\
	l->tail = (l->tail - 1) & (l->size - 1); \
	l->buffer[ l->tail ] = x;                \
}

#define _L_DEF_PEEKF(_name_,_data_t_)   \
__unused _data_t_ list_peekf(_name_)(list_t(_name_) *l) {\
	return l->buffer[(l->head - 1) & (l->size - 1)]; \
}

#define _L_DEF_PEEKB(_name_,_data_t_,_index_t_)  \
__unused _data_t_ list_peekb(_name_)(list_t(_name_) *l) {\
	return l->buffer[l->tail];               \
}

#define _L_DEF_PEEK(_name_,_data_t_,_index_t_)  \
__unused _data_t_ list_peek(_name_)(list_t(_name_) *l, _index_t_ index) {\
	return l->buffer[ (l->head - 1 + index) & (l->size - 1)]; \
}

/* Can be called from non-isr with probably no bad occourances. */
#define _L_DEF_FLUSH(_name_)                             \
__unused void list_flush(_name_)(list_t(_name_) *list) { \
	list->head = list->tail;                         \
}

#define LIST_FLUSH(_list_) do {          \
	(_list_)->head = (_list_)->tail; \
	} while(0)

#define _L_DEF_EMPTY(_name_)                             \
__unused bool list_empty(_name_)(list_t(_name_) *list) { \
	if ( likely( list->head != list->tail ) )        \
		return false;                            \
	else                                             \
		return true;                             \
}

#define LIST_EMPTY(_list_) \
	((_list_)->head == (_list_)->tail)

#define _L_DEF_FULL(_name_)                                                \
__unused bool list_full(_name_)(list_t(_name_) *list) {                    \
	if ( likely( (CIRC_NEXT(list->head,list->size) !=  list->tail )) ) \
		return false;                                              \
	else                                                               \
		return true;                                               \
}

#define LIST_FULL(l) CIRC_FULL((l)->head,(l)->tail,(l)->size)

#define _L_DEF_VAL_I(_name_, _index_t_)  \
__unused bool list_valid_i(_name_)(list_t(_name_) *list, _index_t_ i) {\
	if ( CIRC_CNT(list->head,list->tail,list->size) > i ) \
		return true;  \
	else                  \
		return false; \
}

#define LIST_VALID_I(_l_,_i_) \
	(CIRC_CNT((_l_)->head,(_l_)->tail,(_l_)->size) > (_i_))

#define LIST_DEFINE(_name_,fnattr,dattr,_data_t_,_index_t_) \
	_L_DEF_STRUCT(_name_,dattr,_data_t_,_index_t_)   \
	fnattr _L_DEF_EMPTY (_name_)                     \
	fnattr _L_DEF_FULL  (_name_)                     \
	fnattr _L_DEF_POPF  (_name_,_data_t_)            \
	fnattr _L_DEF_POPB  (_name_,_data_t_)            \
	fnattr _L_DEF_PUSHF (_name_,_data_t_)            \
	fnattr _L_DEF_PUSHB (_name_,_data_t_)            \
	fnattr _L_DEF_PEEKF (_name_,_data_t_)            \
	fnattr _L_DEF_PEEKB (_name_,_data_t_,_index_t_)  \
	fnattr _L_DEF_PEEK  (_name_,_data_t_,_index_t_)  \
	fnattr _L_DEF_VAL_I (_name_,_index_t_)           \
	fnattr _L_DEF_FLUSH (_name_)

#endif
