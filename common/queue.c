/* queue.c
	functions for static FIFO implimentation.
*/
#include "debug.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "queue.h"

void q_init(queue_t *q, QUEUE_BASE_T * b, QUEUE_INDEX_T sz) {
	q->first	= 0;
	q->last	= 0;
	q->ct	= 0;
	q->buffer	= b;
	q->sz	= sz;
}

void q_flush(queue_t *q) {
	q->first = q->last;
	q->ct = 0;

}

QUEUE_BASE_T q_remove(queue_t *q) {
// take the last thing that was put on
	if (q_empty(q)) {
		return '#';
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
	     #if (DEBUG_L(4) && defined(io_isr))
	     fprintf(io_isr,"\n{warn: push (%d)}",x);
	     #endif
		#ifdef Q_OVERWRITE
          q->buffer[ q->last ] = x;
          q->last++;
          if ( q->last >= q->sz )
               q->last = 0;
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

int8_t q_apush(queue_t *q, const QUEUE_BASE_T x[],QUEUE_INDEX_T sz) {
	QUEUE_INDEX_T i;
	for(i=0;i<sz;++i) {
		int8_t ret = q_push(q,x[i]);
		if (ret<0) return (sz-i);
	}
	return 0;
}

QUEUE_BASE_T q_pop(queue_t *q) {
	QUEUE_BASE_T x;

	if (q_empty(q)) {
		#if (DEBUG_L(4) && defined(io_isr))
		fprintf(io_isr,"\n{warn: pop}");
		#endif
		x='#';
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

int q_pop_e(queue_t *q) {
	int x;

	if (q_empty(q)) {
		return EOF;
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

void q_apop(queue_t *q, QUEUE_BASE_T * buffer, QUEUE_INDEX_T sz) {
	QUEUE_INDEX_T i;
	for(i=0;i<sz;++i)
		buffer[i]=q_pop(q);
}

bool q_empty(queue_t *q) {
	if (q->ct <= 0)	return true;
	else	 	return false;
}

bool q_full(queue_t *q) {
	if (q->ct >= q->sz)	return true;
	else 			return false;
}
