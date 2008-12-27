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

int8_t q_push(queue_t *q, QUEUE_BASE_T x)
{
        if (q->ct >= QUEUE_SZ){
		#if DEBUG_L(2)        	
		fprintf_P(stderr,PSTR("Warning: queue overflow push x=%d\n"),x);
		#endif
		return -1;
        }
        else {
                q->buffer[ q->last ] = x;
                q->last = (q->last+1) % (QUEUE_SZ);
		++(q->ct);
        }
	return 0;
}

void q_apush(queue_t *q, const QUEUE_BASE_T x[],QUEUE_INDEX_T sz)
{
	QUEUE_INDEX_T i;
        for(i=0;i<sz && (q_push(q,x[i])>0);++i);
}

QUEUE_BASE_T q_pop(queue_t *q)
{
        QUEUE_BASE_T x;

        if (q->ct <= 0) {
		#if DEBUG_L(2)
		printf_P(PSTR("Warning: empty queue pop.\n"));
		#endif
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
