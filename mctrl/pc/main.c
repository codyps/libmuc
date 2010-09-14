#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include "../proto.h"

ssize_t frame_send(FILE *out, void *data, size_t nbytes)
{
	char *d;
	char *end;

	fputc(START_BYTE, out);

	for(d = data, end = d + nbytes; d < end; d++) {
		char c = *d;
		if (c == START_BYTE || c == ESC_BYTE || c == RESET_BYTE) {
			fputc(ESC_BYTE, out);
			fputc(ESC_MASK ^ c, out);
		} else {
			fputc(c, out);
		}
	}

	fputc(START_BYTE, out);

	fflush(out);
	return nbytes;
}

ssize_t frame_recv(FILE *in, void *vbuf, size_t nbytes)
{
	size_t i;
	char *buf = vbuf;
	bool recv_started = false;
	bool is_escaped = false;

	for(i = 0; i < nbytes; ) {
		int data = fgetc(in);
		if (data == EOF)
			continue;

		if (data == START_BYTE) {
			if (recv_started) {
				if (i != 0)
					return i;
			} else {
				recv_started = true;
			}
			continue;
		}

		if (!recv_started)
			continue;

		if (data == RESET_BYTE) {
			/* restart recv */
			i = 0;
			continue;
		}

		if (data == ESC_BYTE) {
			is_escaped = true;
			continue;
		}

		if (is_escaped) {
			is_escaped = false;
			data ^= ESC_MASK;
		}


		if (i < nbytes) {
			buf[i] = data;
			i++;
		} else {
			return -ENOSPC;
		}
	}
}

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

}

int main(int argc, char **argv)
{

	char *d = "\x7f~";
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
