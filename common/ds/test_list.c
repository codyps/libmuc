/*
Used to test the list implimention.
compile with:
	gcc --std=gnu99 list.c test_list.c -o test_list
*/

#include <stdio.h>
#include <stdint.h>
#include "gcirc.h"
#define LN char

/*     name,fnattr,dattr,dtype,itype */
LIST_DEFINE(LN,static,,char,uint8_t);
char buff[16];
list_t(LN) L = LIST_INITIALIZER(buff);

#define ITER(x) for(size_t i = 0; i < LIST_SZ(*x); i++)

void list_print(list_t(LN) *l) {
	printf("\n{-\ni:\t");
	// the index numbers
	ITER(l) {
		printf("%zd\t",i);
	}
	printf("\nv:\t");
	// the contents (values)
	ITER(l) {
		printf("%d\t", l->buffer[i]);
        }
        printf("\n\t");

	{
	// mark elements which are valid members.
	int list_i = 0;
	int buff_i = l->tail;
	for( int i=0; i< buff_i; i++ )
		printf("\t");
	for( ; list_i < LIST_CT(*l); (list_i++, buff_i++) ) {
		if (buff_i >= LIST_SZ(*l)) {
			buff_i = 0;
			puts("\r\t");
		}
		printf("^\t");
	}
	printf("\n\t");
	}

	// first and end markers.
        ITER(l) {
		bool fst = i==l->head;
		bool end = i==l->tail;
		if (fst && end)
			printf("ht");
		else if (fst)
			printf("h");
		else if (end)
			printf("t");
		else
			printf(" ");
		printf(" \t");
        }
	printf("\n");

	printf("\n");
	// peak function results.
	printf("\tpeek_front : %d\n", list_peekf(LN)(&L));
	printf("\tpeek_back  : %d\n", list_peekb(LN)(&L));

	printf("-}\n\n");
}


int main ( int argc, char **argv ) {

	printf("> map (push_front &L) [0..11] ;\n ");
	for(int i = 0; i <= 11; i++ )
		list_pushf(LN)(&L,i);
	list_print(&L);

	printf("> do (pop_back &L) 10 : \t");
	for(int i = 0; i < 9; i++)
		printf("%d\t",list_popb(LN)(&L));
	list_print(&L);

	printf("> map (push_back &L) [10..17] ;\n ");
	for(int i = 10; i < 18; i++)
		list_pushb(LN)(&L,i);
	list_print(&L);

	printf("> do (pop_front &L) 5 : \t");
	for(int i = 0; i < 5; i++)
		printf("%d\t",list_popf(LN)(&L));
	list_print(&L);

	printf("> map peek &L [0..5] : \t");
	for(int i = 0; i < 6; i++) {
		printf("%d\t",list_peek(LN)(&L,i));
		fflush(stdout);
	}
	list_print(&L);

	return 0;
}
