/* queue.c
	functions for static FIFO implimentation.
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "queue.h"

void q_flush(queue_t *q) {
	q->first = q->last;
	q->ct = 0;
}

QUEUE_BASE_T q_remove(queue_t *q) {
// take the last thing that was put on
	if (q_empty(q)) {
		return 0;
	}
	else {
		QUEUE_BASE_T old = q->buffer[q->last];
		q->ct--;
		q->last--;
		if ( q->last > 0)
			q->last = q->sz;
		return old;
	}
}

int8_t q_push(queue_t *q, QUEUE_BASE_T x)
{
	if (q_full(q)){
	     #if (defined(io_isr))
	     fprintf(io_isr,"\n{warn: push (%d)}",x);
	     #endif
	     return -1;
	}
	else {
		q->buffer[ q->last ] = x;
		q->last++;
		if ( q->last >= q->sz )
			q->last = 0;

		++(q->ct);

          return 0;
	}
}

int8_t q_push_o(queue_t *q, QUEUE_BASE_T x)
{
	q->buffer[ q->last ] = x;
	q->last++;
	if ( q->last >= q->sz )
		q->last = 0;

	if (!q_full(q))
		++(q->ct);
     return 0;
}

QUEUE_BASE_T q_pop(queue_t *q) {
	QUEUE_BASE_T x;

	if (q_empty(q)) {
		#if (defined(io_isr))
		fprintf(io_isr,"\n{warn: pop}");
		#endif
		x=0x00;
	}
	else {
		x = q->buffer[ q->first ];
		q->first++;
		if ( q->first >= q->sz )
			q->first = 0;

		--(q->ct);
	}
	return x;
}

bool q_empty(queue_t *q) {
	if (q->ct <= 0)	return true;
	else	 	return false;
}

bool q_full(queue_t *q) {
	if (q->ct >= q->sz)	return true;
	else 			return false;
}
