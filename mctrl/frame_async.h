#ifndef FRAME_ASYNC_H_
#define FRAME_ASYNC_H_ 1
#include <stdint.h>

void hldc0_init(void);

/* Packet building */
void frame_append_u8(uint8_t n);
void frame_append_u16(uint16_t n);
uint8_t frame_append(void *data, uint8_t len);
void frame_send(void); /* dispatch the constructed packet */

#endif
