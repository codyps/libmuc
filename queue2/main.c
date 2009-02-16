
#include <stdio.h>
#include <string.h>
#include "queue.h"

QUEUE_BASE_T buffer[QUEUE_SZ];
queue_t q;


void q_print(queue_t * q) {
    puts("\nQueue Contents: \n");
    for(QUEUE_INDEX_T i=0;i<(q->sz);++i) {
        if ((q->first)==i)
            putchar('f');
        else
            putchar(' ');
        if ((q->last)==i)
            putchar('l');
        else
            putchar(' ');
    }
    putchar('\n');
    for(QUEUE_INDEX_T i=0;i<(q->sz);++i) {
        putchar(q->buffer[i]);
        putchar(' ');
    }
    putchar('\n');

}

int main(int argv, char **argc) {
	q_init(&q,buffer, QUEUE_SZ);
    memset(buffer,'_',sizeof(buffer));

    q_print(&q);

    q_push(&q,'c');
    q_print(&q);
    putchar(q_pop(&q));
    putchar('\n');

    char test[] = "hello";

    q_apush(&q,test,sizeof(test)-1);

    q_print(&q);

    char test2[sizeof(test)];
    q_apop(&q,test2, sizeof(test)-1);
    test2[sizeof(test)-1]='\0';
    printf("\t%s\n",test2);

    q_apush(&q,test,sizeof(test)-1);

    q_print(&q);

    q_apop(&q,test2, sizeof(test)-1);
    test2[sizeof(test)-1]='\0';
    printf("\t%s\n",test2);

    q_print(&q);


	q_apush(&q,"hello", 6);

    q_print(&q);

	char c [7];
	q_apop(&q,c,6);

	printf("%s\n",c);

	for (uint8_t i = 0;!q_full(&q);i++)
		q_push(&q,'0'+i);

	while (!q_empty(&q))
		printf("%c",q_pop(&q));



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

    q_print(&q);

    char t[] = "1234567890123456789";
    for (uint8_t i=0;i<20;i++) {
        q_apush(&q,t,sizeof(t)-i);
        q_print(&q);
        char t2[sizeof(t)];
        q_apop(&q,t2,sizeof(t)-i);
        printf("pop: %s\n",t2);
    }
	return 0;
}
