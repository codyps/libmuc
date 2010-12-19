#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "frame_async.h"

struct bytebuf {
	char *buf;
	size_t data_len;
	size_t mem_len;
};

struct bytebuf *bytebuf_mk(size_t initial_sz)
{
	struct bytebuf *s = malloc(sizeof(*s));
	if (!s)
		return NULL;

	s->buf = malloc(initial_sz);
	if (!s->buf)
		return NULL;

	s->mem_len = initial_sz;
	s->data_len = 0;
	return s;
}

struct bytebuf *bytebuf_append(struct bytebuf *s, char c) {
	return 0;
}

int main(int argc, char **argv)
{

	char *d = "~";
	char buf[1024];
	frame_send(stdout, d, strlen(d));
	frame_recv(stdin, buf, sizeof(buf));

	fprintf(stderr, "%s\n", buf);
#if 0
	int c;
	for(;;) {
		while((c = fgetc(stdin)) != EOF) {


		}
		if (ferror(stdin)) {
			exit(-2);
		} else if (feof(stdin)) {
			clearerr(stdin);
			send_packet(
		}
	}
#endif
	return 0;
}
