/*
 * list.h
*/
#ifndef _LIST_H_
#define _LIST_H_
#include <stdint.h>
#include <stdbool.h>

typedef char list_base_t;  // allowed to use char* to access any type of mem.
typedef uint8_t list_index_t;

typedef struct {
  LIST_BASE_T *restrict buffer;	// buffer
	LIST_INDEX_T first;	// position of first element
  LIST_INDEX_T end;	// position of last element +1
	LIST_INDEX_T ct;	// number of elements in the list
	const LIST_INDEX_T sz;	// size of the buffer
} list_t;

#define LIST_DEF(buff) {\
  .buffer = (buff), .first = 0, .last = 0, .ct = 0,\
  .sz = sizeof(buff) / sizeof(list_base_t) \
  }

// add to front (push)
#define list_push list_push_front
int8_t list_push_front(list_t *list, list_base_t x);
int8_t list_push_front_o(list_t *list, list_base_t x);

// add to back (append)
#define list_append list_push_back
list_error_t list_push_back(list_t *list, list_base_t x);
list_error_t list_push_back_o(list_t *list, list_base_t x);

// remove
#define list_pop list_pop_front
list_base_t list_pop_front(list_t *list); // (pop)
#define list_take list_pop_back
list_base_t list_pop_back(list_t *list);  // (take)

// peek
list_base_t list_peek_front(list_t *list);
list_base_t list_peek_back(list_t *list);
list_base_t list_peek(list_t *list, list_index_t index);

void list_flush(list_t *list);
bool list_empty(list_t *list);
bool list_full(list_t *list);

#endif

