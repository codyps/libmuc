#ifndef CIRC_BUF_H_
#define CIRC_BUF_H_ 1

#include <stdint.h>

struct circ_buf8 {
	uint8_t *buf;
	uint8_t head; /* location of next removal */
	uint8_t tail; /* location of next insertion */
};

#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size - 1)))

#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))

#define CIRC_CNT_TO_END(head,tail,size) \
	({typeof(head) end = (size) - (tail); \
	  typeof(head) n = ((head) + end) & ((size) - 1); \
	  n < end ? n : end;})

#define CIRC_SPACE_TO_END(head,tail,size) \
	({typeof(head) end = (size) - (tail); \
	  typeof(head) n = ((head) + end) & ((size) - 1); \
	  n < end ? n : end+1;})


#endif
