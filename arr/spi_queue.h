#ifndef SPI_SINGLE_H_
#define SPI_SINGLE_H_

/* Called following the spi transfer, in non-isr context. */
typedef void (*spi_cb_t)(void);

#define WHEN_BEFORE 0
#define WHEN_AFTER 1
/* Prelude and Postlude fn.
 * Intended to be called prior to and following a spi transfer
 * in isr context. */
typedef void (*spi_lude_fn_t)(uint8_t when);

void spi_xfer(uint8_t *dst, uint8_t *src, uint16_t len,
		spi_cb_t *cb, spi_lude_fn_t *between);

#endif
