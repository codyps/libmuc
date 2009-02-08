/* queue.c
	functions for FIFO implimentation.
*/
#include "debug.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "queue.h"

void q_init(queue_t *q, QUEUE_BASE_T * b, QUEUE_INDEX_T sz) {
	q->first	= 0;
	q->last		= 0;
	q->ct		= 0;
	q->buffer	= b;
	q->sz		= sz;
}

int8_t q_push(queue_t *q, QUEUE_BASE_T x)
{
        if ((q->ct) >= (q->sz)){
		#if DEBUG_L(3)
		fprintf(stderr,"warn: q overflow push (%d)\n",x);
		#endif
		return -1;
        }
        else {
                q->buffer[ q->last ] = x;
                q->last = (q->last+1) % (q->sz);
                ++(q->ct);
        }
	return 0;
}

int8_t q_apush(queue_t *q, const QUEUE_BASE_T x[],QUEUE_INDEX_T sz)
{
	QUEUE_INDEX_T i;
    for(i=0;i<sz;++i) {
        int8_t ret = q_push(q,x[i]);
        if (ret<0) return (sz-i);
    }
    return 0;
}

QUEUE_BASE_T q_pop(queue_t *q)
{
        QUEUE_BASE_T x;

        if (q->ct <= 0) {
		#if DEBUG_L(3)
		fprintf(stderr,"warn: empty q pop.\n");
		#endif
		x=QUEUE_BASE_T_MAX;
	}
        else {
                x = q->buffer[ q->first ];
                q->first = (q->first+1) % (q->sz);
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
