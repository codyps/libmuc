#ifndef FRAME_ASYNC_H_
#define FRAME_ASYNC_H_

#include <stdio.h>
ssize_t frame_recv(FILE *in, void *vbuf, size_t nbytes);
ssize_t frame_send(FILE *out, void *data, size_t nbytes);

#endif
