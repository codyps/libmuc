
#include <avr/interrupt.h>
#include <sspi.h>
#include <spi_single.h>

/* transfer control */
static uint8_t *c_dst;
static uint8_t *c_src;
static uint16_t c_len;

static spi_cb_t *c_cb;
static spi_lude_fn_t *c_lude;

/* transfer state */
static uint16_t c_pos;

void spi_init(void)
{
	sspi_master_init();
}

static void spi_start_xfer(void)
{

}

void spi_main_handler(void)
{
	if(c_len && (c_pos == c_len)) {
		spi_cb_t scb = c_cb;
		c_len = 0;
		c_dst = 0;
		c_src = 0;
		c_cb = 0;
		c_lude = 0;
		c_pos = 0;

		if (scb)
			scb();
	}
}

void spi_xfer(uint8_t *dst, uint8_t *src, uint16_t len, spi_cb_t *cb,
		spi_lude_fn_t *lude)
{
	c_dst = dst;
	c_src = src;
	c_len = len;
	c_cb = cb;
	c_lude = lude;
	c_pos = 0;

	spi_start_xfer();
}

ISR(SPI_vect)
{
	
}
