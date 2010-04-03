#ifndef LIST_H_
#define LIST_H_

#include <stdint.h>
#include <stdbool.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

typedef int8_t list_error_t;

#define LIST_ERROR_NORET LIST_ERROR
#define LIST_ERROR(...) fprintf(stderr,"\nERROR: %s:%s():%d \n",__FILE__,__func__,__LINE__)

#define list_t(_name_) list_##_name_##_t

/* first = first char, end = last + 1,
 * ct = nmem, sz = max_nmem */
#define _L_DEF_STRUCT(_name_,_data_t_,_index_t_) \
	typedef struct {        \
		_data_t_ *buffer;   \
		_index_t_ first;    \
		_index_t_ end;      \
		_index_t_ ct;       \
		const _index_t_ sz; \
	} list_t(_name_);
	
#define LIST_INITIALIZER(buff) {       \
	.buffer = (buff), .first = 0,      \
	.end = 0, .ct = 0,                 \
	.sz = sizeof(buff) / sizeof(*buff) \
	}

#define list_popf(_name_) list_##_name_##_pop_front
#define list_popb(_name_) list_##_name_##_pop_back

#define list_pushf(_name_) list_##_name_##_push_front
#define list_pushb(_name_) list_##_name_##_push_back
#define list_pushfo(_name_) list_##_name_##_push_front_o

#define list_peekb(_name_) list_##_name_##_peek_back
#define list_peekf(_name_) list_##_name_##_peek_front
#define list_peek(_name_) list_##_name_##_peek

#define list_flush(_name_) list_##_name_##_flush
#define list_full(_name_) list_##_name_##_full
#define list_empty(_name_) list_##_name_##_empty
#define list_valid_i(_name_) list_##_name_##_valid_index

#define _L_DEF_POPF(_name_,_data_t_)         \
_data_t_ list_popf(_name_)(list_t(_name_) *l) { \
	if ( unlikely( list_empty(_name_)(l) ) ) {       \
		LIST_ERROR_NORET();                  \
		return 0;                            \
	}                                        \
	_data_t_ head = l->buffer[ l->first ];   \
	l->first++;                              \
	if ( l->first >= l->sz )                 \
		l->first = 0;                        \
	--(l->ct);                               \
	return head;                             \
}

/* the (l->end >= l->sz) is a check for overflow. */
#define _L_DEF_POPB(_name_,_data_t_)        \
_data_t_ list_popb(_name_)(list_t(_name_) *l) { \
	if ( unlikely( list_empty(_name_)(l) ) ) {  \
		LIST_ERROR_NORET();                 \
		return 0;                           \
	}                                       \
	l->ct--;                                \
	l->end--;                               \
	if ( l->end >= l->sz)                   \
		l->end = l->sz - 1;                 \
	_data_t_ last = l->buffer[l->end];      \
	return last;                            \
}

#define _L_DEF_PUSHF(_name_,_data_t_)   \
list_error_t list_pushf(_name_)(list_t(_name_) *l, _data_t_ x) { \
	if ( unlikely( list_full(_name_)(l) ) ){ \
		LIST_ERROR();                \
		return -1;                   \
	}                                \
	l->ct++;                         \
	l->first--;                      \
	if ( l->first >= l->sz )         \
		l->first = l->sz - 1;        \
	l->buffer[ l->first ] = x;       \
	return 0;                        \
}

#define _L_DEF_PUSHFO(_name_,_data_t_)\
void list_pushfo(_name_)(list_t(_name_) *l, _data_t_ x) {\
	if ( !list_full(_name_)(l) ) {     \
		l->ct++;               \
	}                          \
	l->first--;                \
	if( l->first >= l->sz )    \
		l->first = l->sz - 1;  \
	l->buffer[ l->first ] = x; \
}

#define _L_DEF_PUSHB(_name_,_data_t_) \
list_error_t list_pushb(_name_)(list_t(_name_) *l, _data_t_ x) {\
	if ( unlikely( list_full(_name_)(l) ) ){ \
		LIST_ERROR();                \
		return -1;                   \
	}                                \
	l->ct++;                         \
	l->buffer[ l->end ] = x;         \
	l->end++;                        \
	if ( l->end >= l->sz )           \
		l->end = 0;                  \
	return 0;                        \
}

#define _L_DEF_PUSHBO(_name_,_data_t_)\
void list_pushbo(_name_)(list_t(_name_) *l, _data_t_ x) {\
	l->buffer[ l->end ] = x; \
	l->end++;                \
	if ( l->end >= l->sz )   \
		l->end = 0;          \
	if (!list_full(l))       \
		++(l->ct);           \
	return 0;                \
}

#define _L_DEF_PEEKF(_name_,_data_t_)   \
_data_t_ list_peekf(_name_)(list_t(_name_) *l) {\
	if ( unlikely( list_empty(_name_)(l) ) ) {  \
		LIST_ERROR_NORET();             \
		return 0;                       \
	}                                   \
	return l->buffer[l->first];         \
}

#define _L_DEF_PEEKB(_name_,_data_t_,_index_t_)  \
_data_t_ list_peekb(_name_)(list_t(_name_) *l) {\
	if ( unlikely( list_empty(_name_)(l) ) ) { \
		LIST_ERROR_NORET();            \
		return 0;                      \
	}                                  \
	_index_t_ last = l->end-1;         \
	if (last >= l->sz) {               \
		last = l->sz-1;                \
	}                                  \
	return l->buffer[last];            \
}

#define _L_DEF_PEEK(_name_,_data_t_,_index_t_)  \
_data_t_ list_peek(_name_)(list_t(_name_) *l, _index_t_ index) {\
	if ( unlikely( index >= l->ct ) ) {         \
		LIST_ERROR_NORET();                     \
		return 0;                               \
	}                                           \
	_index_t_ list_i, buff_i;                   \
	for( (buff_i = l->first, list_i = 0) ; ; (++buff_i,++list_i) ) {\
		if (buff_i >= l->sz)                    \
			buff_i = 0;                         \
		if (list_i == index)                    \
			return l->buffer[buff_i];           \
	}                                           \
	LIST_ERROR_NORET();                         \
	return 0;                                   \
}

/* Can be called from non-isr with probably no bad occourances. */
#define _L_DEF_FLUSH(_name_)    \
void list_flush(_name_)(list_t(_name_) *list) { \
	list->end = list->first;    \
	list->ct = 0;               \
}

#define _L_DEF_EMPTY(_name_)            \
bool list_empty(_name_)(list_t(_name_) *list) { \
	if ( likely( list->ct != 0 ) )      \
		return false;                   \
	else	 	                        \
		return true;                    \
}

#define _L_DEF_FULL(_name_)              \
bool list_full(_name_)(list_t(_name_) *list) {   \
	if ( likely( list->ct < list->sz ) ) \
		return false;                    \
	else                                 \
		return true;                     \
}

#define _L_DEF_VAL_I(_name_, _index_t_)  \
bool list_valid_i(_name_)(list_t(_name_) *list, _index_t_ i) {\
	if ( likely( list->ct > i && i > 0 ) ) \
		return true;                       \
	else                                   \
		return false;                      \
}

#define LIST_DEFINE(_name_,_data_t_,_index_t_) \
	_L_DEF_STRUCT(_name_,_data_t_,_index_t_)   \
	_L_DEF_EMPTY (_name_)                      \
	_L_DEF_FULL  (_name_)                      \
	_L_DEF_POPF  (_name_,_data_t_)             \
	_L_DEF_POPB  (_name_,_data_t_)             \
	_L_DEF_PUSHF (_name_,_data_t_)             \
	_L_DEF_PUSHB (_name_,_data_t_)             \
	_L_DEF_PUSHFO(_name_,_data_t_)             \
	_L_DEF_PEEKF (_name_,_data_t_)             \
	_L_DEF_PEEKB (_name_,_data_t_,_index_t_)   \
	_L_DEF_PEEK  (_name_,_data_t_,_index_t_)   \
	_L_DEF_VAL_I (_name_,_index_t_)            \
	_L_DEF_FLUSH (_name_)

#endif