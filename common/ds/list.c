/* list.c
	functions for static list implimentation.
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "list.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define LIST_ERROR_NORET LIST_ERROR
#define LIST_ERROR(...) fprintf(stderr,"\nERROR: %s:%s():%d \n",__FILE__,__FUNCTION__,__LINE__);


list_base_t list_pop_front(list_t *l) {
	if ( unlikely( list_empty(l) ) ) {
		LIST_ERROR_NORET();
		return 0;
	}

	list_base_t head = l->buffer[ l->first ];

	l->first++;
	if ( l->first >= l->sz )
		l->first = 0;

	--(l->ct);

	return head;
}

list_base_t list_pop_back(list_t *l) {
	if ( unlikely( list_empty(l) ) ) {
		LIST_ERROR_NORET();
		return 0;
	}

	l->ct--;

	l->end--;
	if ( l->end >= l->sz) // if overflowed
		l->end = l->sz - 1;

	list_base_t last = l->buffer[l->end];
  
	return last;
}

list_error_t list_push_front(list_t *l, list_base_t x) {
	if ( unlikely( list_full(l) ) ){
		LIST_ERROR();
		return -1;
	}

	l->ct++;

	l->first--;
	if ( l->first >= l->sz ) // if overflowed
		l->first = l->sz - 1;

	l->buffer[ l->first ] = x;

	return 0;
}

void list_push_front_o(list_t *l, list_base_t x) {
	if ( !list_full(l) ) {
		l->ct++;
	}

	l->first--;
	if( l->first >= l->sz )
		l->first = l->sz - 1;

	l->buffer[ l->first ] = x;

}

list_error_t list_push_back(list_t *l, list_base_t x) {
	if ( unlikely( list_full(l) ) ){
		LIST_ERROR();    
		return -1;
	}

	l->ct++;

	l->buffer[ l->end ] = x;

	l->end++;
	if ( l->end >= l->sz ) 
		l->end = 0;

	return 0;
}

void list_push_back_o(list_t *l, list_base_t x) {
	l->buffer[ l->end ] = x;

	l->end++;
	if ( l->end >= l->sz )
		l->end = 0;

	if (!list_full(l))
		++(l->ct);
	
	return;
}

list_base_t list_peek_front(list_t *l) {
	if ( unlikely( list_empty(l) ) ) {
		LIST_ERROR_NORET();
		return 0;
	}
	
	return l->buffer[l->first];
}

list_base_t list_peek_back(list_t *l) {
	if ( unlikely( list_empty(l) ) ) {
		LIST_ERROR_NORET();
		return 0;
	}
	
	list_index_t last = l->end-1;
	if (last >= l->sz) {
		last = l->sz-1;
	}
	
	return l->buffer[last];
}

list_base_t list_peek(list_t *l, list_index_t index) {
	if ( unlikely( index >= l->ct ) ) {
		LIST_ERROR_NORET();
		return 0;
	}

	list_index_t list_i, buff_i;
	for( (buff_i = l->first, list_i = 0) ; ; (++buff_i,++list_i) ) {
		if (buff_i >= l->sz)
			buff_i = 0;
		if (list_i == index)
			return l->buffer[buff_i];

	}

	LIST_ERROR_NORET();
	return 0;
}

/* Can be called from non-isr with probably no bad occourances. */
void list_flush(list_t *list) {
	list->end = list->first;
	list->ct = 0;
}

bool list_empty(list_t *list) {
	if ( likely( list->ct != 0 ) )	return false;
	else	 	return true;
}

bool list_full(list_t *list) {
	if ( likely( list->ct < list->sz ) )	return false;
	else 			return true;
}

bool list_valid_index(list_t *list, list_index_t i) {
	if ( likely( list->ct > i && i > 0 ) ) return true;
	else return false;

}

