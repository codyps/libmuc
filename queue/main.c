
#include <stdio.h>
#include "queue.h"

queue_t q;

int main(int argv, char **argc) {
	q_init(&q);
	q_apush(&q,"hello", 5);
	
	char c [7];
	q_apop(&q,&c,5);
	c[6]='\0';
	
	printf("%s\n",c);
	
	for (uint8_t i = 0;!q_full(&q);i++)
		q_push(&q,'0'+i);
	
	while (!q_empty(&q))
		printf("%c",q_pop(&q));
	
	/*
	
	printf("\n");
	for (uint8_t i=0;i<=('9'-'0'+25);i++) {
		while (!q_full(&q))
			q_push(&q,'0'+i);
		
		while (q.ct>3)
			printf("%c",q_pop(&q));
		printf("\n");
	}
	
	
	while (!q_empty(&q))
		printf("%c",q_pop(&q));
*/
	printf("%d\n",q.first);
	return 0;
}