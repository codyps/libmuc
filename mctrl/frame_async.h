#ifndef FRAME_ASYNC_H_
#define FRAME_ASYNC_H_ 1
#include <stdint.h>
#include <stdbool.h>

void frame_init(void);

#if defined(DEBUG)
/* debug */
void frame_timeout(void);
#endif

#ifndef AVR
/* For simulating interrupts on non-avr hardware */
void frame_tx_isr(void);
void frame_rx_isr(void);
#endif

/*** Transmision ***/
/** Packet building **/
/* Note that if buffer space runs out at any point durring building the
 * packet will be dropped without indication to the user. The next packet
 * will not be started until another call to frame_start.
 */

/* begin construction of a packet */
void frame_start(void);

/* various appends */
void frame_append_u8(uint8_t n);
void frame_append_u16(uint16_t n);

/* dispatch the constructed packet */
void frame_done(void);

/** Full Packet transmit **/
void frame_send(const void *data, uint8_t nbytes);

/*** Reception ***/

/* 2 paths possible for recviever:
 *  - call frame_recv_len (returns 0 if no packet is ready) to see if there is
 *    a packet, subsequently recv data via frame_recv_byte & frame_recv_copy.
 *  - call frame_recv_copy (returns 0 if no packet) and either have all data
 *    returned in the single copy call, or use subsequent
 *    frame_recv_{copy,byte} calls to get all the data.
 * Once done recieving current packet, call frame_recv_next to advance to the
 * next packet. Packets *are not* advanced automatically under any condition.
 */

/* dst: array of at least len bytes into which the current packet is copied
 *      (up to len bytes).
 * return: length of current packet, not the amount copied into dst.
 * byte pointer is advanced by MIN(len, packet_len)
 */
uint8_t frame_recv_copy(uint8_t *dst, uint8_t dst_len);

/* return: number of bytes in the current packet. 0 indicates the lack of
 * a packet (packets cannot be 0 bytes).
 */
uint8_t frame_recv_len(void);

/* return: next byte from packet.
 * advances byte pointer.
 *
 * it is not recommended to call this without first determining that the
 * next byte in the packet exsists. It will return 0 in the case were
 * no bytes remain in the packet, which may be a valid value in the
 * packet and thus should not be used to indicate the packet is
 * fully processed.
 */
uint8_t frame_recv_byte(void);

/* advance the packet pointer to the next packet.
 *
 * One must be sure the packet queue is not empty prior to calling this func.
 * That may be done either
 *  - by calling frame_recv_ct (non-zero when the queue is non-empty or
 *  - by only calling this func when one finishes processing the current packet
 *    which ensures that at least one packet (the one presently being
 *    processed) will be in the packet queue.
 */
void frame_recv_next(void);

/* return: true if at least one packet is in the queue. The queue includes
 *         the packet currently being processed.
 */
bool frame_recv_have_pkt(void);

/* return: the number of packets presently in the queue. The queue includes
 *         the packet currently being processed.
 */
uint8_t frame_recv_ct(void);

#endif
