#ifndef CIRC_BUF_H_
#define CIRC_BUF_H_ 1

#include <stdint.h>

struct circ_buf8 {
	uint8_t *buf;
	uint8_t head; /* location of next insertion */
	uint8_t tail; /* location of next removal */
};

/* number of items in circ_buf */
#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size)- 1))

/* space remaining in circ_buf */
#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))

/* is circ_buf full */
#define CIRC_FULL(head,tail,size) (CIRC_NEXT(tail,size) == (head))

/* next index (head/tail) location */
#define CIRC_NEXT(index,size) CIRC_NEXT_I(index,1,size)
#define CIRC_NEXT_I(index,isz,size) (((index) + (isz)) & ((size) - 1))

/* assign next index (head/tail) location to index */
#define CIRC_NEXT_EQ(index,size) CIRC_NEXT_I_EQ(index,1,size)
#define CIRC_NEXT_I_EQ(index,isz,size) ((index) = (((index) + (isz)) & ((size - 1))))

#define CIRC_CNT_TO_END(head,tail,size) \
	({typeof(head) end = (size) - (tail); \
	  typeof(head) n = ((head) + end) & ((size) - 1); \
	  n < end ? n : end;})

#define CIRC_SPACE_TO_END(head,tail,size) \
	({typeof(head) end = (size) - (tail); \
	  typeof(head) n = ((head) + end) & ((size) - 1); \
	  n < end ? n : end+1;})


#endif
