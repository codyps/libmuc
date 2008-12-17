/* queue.c
	functions for FIFO implimentation.
*/
#include <stdio.h>
#include <stdbool.h>
#include "queue.h"

void q_init(queue_t *q) {
        q->first	= 0;
        q->last		= 0;
	q->ct		= 0;
}

void q_push(queue_t *q, QUEUE_BASE_T x)
{
        if (q->ct >= QUEUE_SZ){
        	printf("Warning: queue overflow push x=%d\n",x);
        }
        else {
                q->buffer[ q->last ] = x;
                q->last = (q->last+1) % (QUEUE_SZ-1);
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

        if (q->ct <= 0) printf("Warning: empty queue pop.\n");
        else {
                x = q->buffer[ q->first ];
                q->first = (q->first+1) % (QUEUE_SZ-1);
		--(q->ct);
        }

        return x;
}

void q_apop(queue_t *q, QUEUE_BASE_T * buffer, QUEUE_INDEX_T sz) {
	QUEUE_INDEX_T i;
	for(i=0;i<sz;++i)
		buffer[i]=q_pop(q);
}


uint8_t q_empty(queue_t *q) {
    if (q->ct <= 0) return true;
    else 			return false;
}

uint8_t q_full(queue_t *q) {
	if (q->ct >= QUEUE_SZ)	return true;
	else 					return false;
}

void q_print(queue_t *q) {

}