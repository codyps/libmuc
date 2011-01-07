#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

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
			break;

		if (data < 0) {
			return data;
		}

		if (ferror(in)) {
			return -255;
		}

		if (data == START_BYTE) {
			if (recv_started) {
				if (i != 0) {
					ungetc(data, in);
					return i;
				}
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
	return i;
}
