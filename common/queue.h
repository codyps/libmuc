
/*
 * queue.h
*/
#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stdint.h>
#include <stdbool.h>

#define QUEUE_BASE_T	uint8_t
#define QUEUE_INDEX_T	uint8_t
#define QUEUE_SZ	64

typedef volatile struct {
     QUEUE_BASE_T * buffer;	// buffer
     QUEUE_INDEX_T first;	// position of first element
	QUEUE_INDEX_T last;	// position of last element (should be eliminated)
	QUEUE_INDEX_T ct;	//
	QUEUE_INDEX_T sz;	// size of the buffer
}  queue_t;


void q_init(queue_t *q, QUEUE_BASE_T * buff, QUEUE_INDEX_T sz);
int8_t q_push(queue_t *q, QUEUE_BASE_T x);
int8_t q_apush(queue_t *q, const QUEUE_BASE_T x[],QUEUE_INDEX_T sz);
QUEUE_BASE_T q_pop(queue_t *q);
void q_flush(queue_t *q);
QUEUE_BASE_T q_remove(queue_t *q);
void q_apop(queue_t *q, QUEUE_BASE_T * buffer, QUEUE_INDEX_T sz);
bool q_empty(queue_t *q);
bool q_full(queue_t *q);

#endif
