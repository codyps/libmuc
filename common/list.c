/* list.c
	functions for static list implimentation.
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "list.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define LIST_ERROR(...) fprintf(stderr,"ERROR: %s:%s():%d \n",__FILE__,__FUNCTION__,__LINE__);


list_base_t list_pop_front(list_t *l) {
	if ( unlikely( list_empty(l) ) ) {
		LIST_ERROR();
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
		LIST_ERROR();
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

list_error_t list_push_back_o(list_t *l, list_base_t x) {
	l->buffer[ l->end ] = x;

	l->end++;
	if ( l->end >= l->sz )
		l->end = 0;

	if (!list_full(l))
		++(l->ct);
	
	return 0;
}

list_base_t list_peek_front(list_t *l) {
	if ( unlikely( list_empty(l) ) ) {
		LIST_ERROR();
		return 0;
	}
	
	return l->buffer[l->first];
}
list_base_t list_peek_back(list_t *l) {
	if ( unlikely( list_empty(l) ) ) {
		LIST_ERROR();
		return 0;
	}
	
	list_index_t last = l->end-1;
	if (last >= l->sz) {
		last = l->sz-1;
	}
	
	return l->buffer[last];
}



list_base_t list_peek(list_t *l, list_index_t index) {
	if ( unlikely( index > l->ct ) ) {
		LIST_ERROR();
		return 0;
	}

	uint8_t sum = l->first + index;
	uint8_t diff = l->first - index;

	if ( sum < l->first ) {
		//XXX: damn overflow.
	}
  
	if ( sum >= l->sz ) {

	}
}

void list_flush(list_t *list) {
	list->first = list->end;
	list->ct = 0;
}

bool list_empty(list_t *list) {
	if ( unlikely( list->ct == 0 ) )	return true;
	else	 	return false;
}

bool list_full(list_t *list) {
	if ( unlikely( list->ct >= list->sz ) )	return true;
	else 			return false;
}
