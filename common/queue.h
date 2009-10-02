/*
 * queue.h
*/
#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stdint.h>
#include <stdbool.h>

#define QUEUE_BASE_T	uint8_t
#define QUEUE_INDEX_T	uint8_t

typedef volatile struct {
	QUEUE_BASE_T *restrict buffer;	// buffer
	QUEUE_INDEX_T first;	// position of first element
	QUEUE_INDEX_T last;	// position of last element (should be eliminated)
	QUEUE_INDEX_T ct;	//
	const QUEUE_INDEX_T sz;	// size of the buffer
} queue_t;

#define Q_DEF(buff) { .buffer = (buff), .first = 0, .last = 0, .ct = 0, .sz = sizeof(buff) / sizeof(QUEUE_BASE_T) }

// adds a new element
int8_t q_push(queue_t *q, QUEUE_BASE_T x);
int8_t q_push_o(queue_t *q, QUEUE_BASE_T x); // overwrites old data on q full

QUEUE_BASE_T q_pop(queue_t *q); // takes oldest element
QUEUE_BASE_T q_remove(queue_t *q); // takes newest element

void q_flush(queue_t *q);
bool q_empty(queue_t *q);
bool q_full(queue_t *q);

#endif

