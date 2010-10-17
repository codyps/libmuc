#ifndef FRAME_ASYNC_H_
#define FRAME_ASYNC_H_ 1
#include <stdint.h>

void frame_init(void);

#if defined(DEBUG)
/* debug */
void frame_timeout(void);
#endif

#ifndef AVR
void frame_tx_isr(void);
void frame_rx_isr(void);
#endif

/** Transmision **/
/* Packet building */
void frame_start(void);
void frame_append_u8(uint8_t n);
void frame_append_u16(uint16_t n);
void frame_done(void); /* dispatch the constructed packet */

/* Full Packet transmit */
void frame_send(const void *data, uint8_t nbytes);

/** Reception **/

/* Copy the packet into dst[], which hase length dst_len.
 * return the actual length of the packet. */
uint8_t frame_recv_copy(uint8_t *dst, uint8_t dst_len);

/* length of current recv packet. Intended to be used with frame_recv_byte */
uint8_t frame_recv_len(void);
/* returns the next byte in the current packet.
 * Returns 0 when packet is empty */
uint8_t frame_recv_byte(void);

/* discard the current recv packet */
void frame_recv_next(void);
/* number of packets currently waiting in queue */
uint8_t frame_recv_ct(void);

#endif
