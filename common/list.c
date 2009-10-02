/* list.c
	functions for static list implimentation.
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "list.h"

list_base_t list_pop_front(list_t *l) {

	if (list_empty(l)) {
		#if (defined(io_isr))
		fprintf(io_isr,"\n{warn: pop}");
		#endif
		return 0x00;
	}

	list_base_t head = l->buffer[ l->first ];

	l->first++;
	if ( l->first >= l->sz )
		l->first = 0;

	--(l->ct);

	return head;
}

list_base_t list_pop_back(list_t *l) {

	if (list_empty(l)) {
    // error.
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
	if (list_full(l)){
	     #if (defined(io_isr))
	     fprintf(io_isr,"\n{warn: push (%d)}",x);
	     #endif
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
	if (list_full(l)){
	     #if (defined(io_isr))
	     fprintf(io_isr,"\n{warn: push (%d)}",x);
	     #endif
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

list_bast_t list_peek(list_t *l, list_index_t index) {
  if ( index > l->ct ) {
    // errorz    
    return 0;
  }

  uint8_t sum = l->first + index;
  uint8_t diff = l->first - index;

  if ( sum < first ) {
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
	if (list->ct == 0)	return true;
	else	 	return false;
}

bool list_full(list_t *list) {
	if (list->ct >= list->sz-1)	return true;
	else 			return false;
}
