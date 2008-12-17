
/*	
 * queue.h
*/

#include <stdint.h>

#define QUEUE_BASE_T	char
#define QUEUE_INDEX_T	uint8_t
#define QUEUE_SZ		10

typedef struct {
        QUEUE_BASE_T  buffer[QUEUE_SZ];	/* buffer */
        QUEUE_INDEX_T first;			/* position of first element */
        QUEUE_INDEX_T last;				/* position of last element */
	QUEUE_INDEX_T ct;
} queue_t;


void q_init(queue_t *q);
void q_push(queue_t *q, QUEUE_BASE_T x);
void q_apush(queue_t *q, const QUEUE_BASE_T x[],QUEUE_INDEX_T sz);
QUEUE_BASE_T q_pop(queue_t *q);
void q_apop(queue_t *q, QUEUE_BASE_T * buffer, QUEUE_INDEX_T sz);
uint8_t q_empty(queue_t *q);
uint8_t q_full(queue_t *q);