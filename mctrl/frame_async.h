#ifndef FRAME_ASYNC_H_
#define FRAME_ASYNC_H_ 1
#include <stdint.h>

void frame_init(void);

/* Packet building */
void frame_start(void);
void frame_append_u8(uint8_t n);
void frame_append_u16(uint16_t n);
void frame_done(void); /* dispatch the constructed packet */

/* Full Packet transmit */
void frame_send(const void *data, uint8_t nbytes);

/** Reception **/
/* pointer to packer returned. NULL if no packet avaliable */
uint8_t *frame_recv(void);
/* length of current recv packet. Only rely on result if frame_recv is valid */
uint8_t frame_recv_len(void);
/* discard the current recv packet */
void frame_recv_drop(void);
/* number of packets currently waiting in queue */
uint8_t frame_recv_ct(void);

#endif
