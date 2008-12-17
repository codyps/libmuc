/* queue.c
	functions for FIFO implimentation.
*/
#include <stdio.h>
#include <stdbool.h>
#include "queue.h"
#include <avr/pgmspace.h>

void q_init(queue_t *q) {
        q->first	= 0;
        q->last		= 0;
	q->ct		= 0;
}

void q_push(queue_t *q, QUEUE_BASE_T x)
{
        if (q->ct >= QUEUE_SZ){
        	printf_P(PSTR("Warning: queue overflow push x=%d\n"),x);
        }
        else {
                q->buffer[ q->last ] = x;
                q->last = (q->last+1) % (QUEUE_SZ);
		++(q->ct);
        }
}

void q_apush(queue_t *q, const QUEUE_BASE_T x[],QUEUE_INDEX_T sz)
{
	QUEUE_INDEX_T i;
        for(i=0;i<sz;++i)
        	q_push(q,x[i]);
}

QUEUE_BASE_T q_pop(queue_t *q)
{
        QUEUE_BASE_T x;

        if (q->ct <= 0) {
		printf_P(PSTR("Warning: empty queue pop.\n"));
		x=QUEUE_BASE_T_MAX;
	}
        else {
                x = q->buffer[ q->first ];
                q->first = (q->first+1) % (QUEUE_SZ);
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
	if (q->ct >= QUEUE_SZ)return true;
	else 			return false;
}
