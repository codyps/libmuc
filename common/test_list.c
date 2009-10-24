/*
Used to test the list implimention.
compile with:
	gcc --std=gnu99 list.c test_list.c -o test_list
*/

#include <stdio.h>
#include "list.h"

uint8_t buff[10];
list_t L = LIST_INITIALIZER(buff);

#define ITER(x) for(int i = 0; i < x->sz; i++)

void list_print(list_t *l) {
	printf("\n{-\n\t");
        ITER(l) {
		printf("%d\t",i);
	}
	printf("\n\t");

	ITER(l) {
		printf("%d\t", l->buffer[i]);
        }
        printf("\n\t");
        
	{
	int list_i = 0;
	int buff_i = l->first;
	for( ; list_i < l-> ct; (list_i++, buff_i++) ) {
		if (buff_i >= l->sz)
			buff_i = 0;
		printf("^\t");
	}
	printf("\n\t");
	}
	
        ITER(l) {
		bool fst = i==l->first;
		bool end = i==l->end;
		if (fst && end) 
			printf("fe");
		else if (fst)
			printf("f");
		else if (end)
			printf("e");
		else
			printf(" ");
		printf(" \t");
        }
	printf("\n");
	
	
	printf("\n");
	printf("\tpeek_front : %d\n", list_peek_front(&L));
	printf("\tpeek_back  : %d\n", list_peek_back(&L));
	
	printf("-}\n\n");
}


int main ( int argc, char **argv ) {
	for (int i = 0; i < 10; i++ )
		list_push_front(&L,i);
	list_print(&L);
	printf(" poping: ");
	for (int i = 0; i < 9; i++)
		printf("%d\t",list_pop_back(&L));
	printf("\n");
	list_print(&L);
	
	return 0;
}
