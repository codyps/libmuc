#include "circ_buf.h"

#include <stdio.h>

void print_info(uint8_t head, uint8_t tail, uint8_t size)
{
	printf("head (insertion point): %d\n", head);
	printf("tail (removal   point): %d\n", tail);
	printf("size : %d\n", size);
	printf("space_to_end: %d\n", CIRC_SPACE_TO_END(head, tail, size));
	printf("cnt_to_end:   %d\n", CIRC_CNT_TO_END(head, tail, size));
	printf("cnt:   %d\n", CIRC_CNT(head, tail, size));
	printf("space:   %d\n", CIRC_SPACE(head, tail, size));
}

int main(int argc, char **argv)
{
	uint8_t head = 0, tail = 0, size = 8;

	for(head = 0; head < size; head++) {
		print_info(head, tail, size);
	}


	return 0;
}
