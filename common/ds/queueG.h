/*
 * queue.h
*/
#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stdint.h>
#include <stdbool.h>

#define QUEUE_BASE_PT	char* //(char* can be used to access any data type)
#define QUEUE_INDEX_T	uint8_t
#define QUEUE_SZ	64

typedef volatile struct {
	QUEUE_BASE_PT buffer;	// buffer
	QUEUE_INDEX_T first;	// position of first element
	QUEUE_INDEX_T last;	// position of last element (should be eliminated)
	QUEUE_INDEX_T ct;	//
	const QUEUE_INDEX_T len;	// length of the buffer
	const QUEUE_INTEX_T item_sz;
	//uint8_t flags;      // for when i need to tack something on.
}  queue_t;

#define Q_DEF(buff) { .buffer = (buff), .first = 0, .last = 0, .ct = 0, \
	.buffer_sz = sizeof(buff) / sizeof( *(buff) ),\
	.item_sz = sizeof( *(buff) ) }

#define Q_DEF2(buff,len,sz) { .buffer = (buff), .first = 0, .last = 0, .ct = 0, \
	.len = (len),\
	.item_sz = (sz) }

QUEUE_BASE_PT q_peak(queue_t *q, QUEUE_INDEX_T pos);
QUEUE_BASE_PT q_peak_fst(queue_t *q);

QUEUE_BASE_PT q_get(queue_t *q); //remove from bottom
QUEUE_BASE_PT q_pop(queue_t *q); //remove from top

// add to bottom.
int8_t q_put(queue_t *q, QUEUE_BASE_T x);
int8_t q_put_o(queue_t *q, QUEUE_BASE_T x); // overwrites old data on q full

// add to top.
int8_t q_push(queue_t *q, QUEUE_BASE_T x);
int8_t q_push_o(queue_t *q, QUEUE_BASE_T x); // overwrites old data on q full

void q_flush(queue_t *q);
bool q_empty(queue_t *q);
bool q_full(queue_t *q);

#endif

