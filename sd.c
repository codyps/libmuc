#include <stdio.h>
#include <avr/io.h>

#include "spi_master.h"
#include "sd_conf.h"

static FILE sd_files[SD_FILE_CT];
static uint8_t sd_files_pos;

#define sd_cs_release() SD_CS_PORT |= (1 << SD_CS_IX)
#define sd_cs_assert()   SD_CS_PORT &= ~(1 << SD_CS_IX)

/* MSB first.
 * CRC = 16 bit CCITT
 */

void sd_host_init(void)
{
	spi_master_init();
	/* SD CS = output */
	DDR(SD_CS_PORT) |= (1 << SD_CS_IX);
	sd_cs_release();
}

#define sd_send_array(a) sd_send_bytes(ARRAY_SIZE(a), a)
static void sd_send_bytes(uint8_t len, uint8_t *bytes)
{
	uint8_t i;
	for (i = 0; i < len; i++) {
		spi_xfer_discard(bytes[i]);
	}
}

static uint8_t sd_send_cmd_ignore_r1(uint8_t cmd, uint8_t argument);
static uint8_t sd_send_cmd_ignore_r2(uint8_t cmd, uint8_t argument);
static uint8_t sd_send_cmd_ignore_r3(uint8_t cmd, uint8_t argument);
static uint8_t sd_send_cmd_r1(uint8_t cmd, uint8_t argument, uint8_t *r1);

static void card_to_spi_mode(void)
{
	/* card must be in IDLE or any state in SPI mode */

	sd_cs_assert();
	/* XXX: may need to setup SDHC/SDXC by sending some additional commands
	 * before going to SPI mode */
	sd_send_cmd(CMD0, { 0, 0, 0, 42} );
	sd_cs_release();

	/* now in SPI operation mode */
}

void sd_card_init(void)
{
	/* Card identification mode */
	card_to_spi_mode();

	sd_send_cmd(CMD8, ...); // mandatory for spec 2.00.
	// if Illegal cmd: v1 or not SD
	// check voltage range in returned data. Retry CMD8.

	sd_send_cmd(CMD58, ...); // optional, READ OCR
	// if invalid command, not a SD memory card.
	do {
		sd_send_acmd(ACMD41, ...);
		// if invalid command, not a SD memory card.

	} while(/* card is busy */);

	sd_send_cmd(CMD58, ...); // Get CCS

	/* Now in data transfer mode */
}

void sd_reset(void)
{
	/* CMD0 = set to idle state
	 * CMD8 = check for SD 2.0 support
	 * ACMD41 = check a bunch of crap
	 */

}

struct acmd41_arg {
	/* 31   : busy     :1
	 * 30   : HCS      :1
	 * 29   : FB       :1
	 * 27   : XPC      :1
	 * 26-24: reserved :3
	 * 23   : S18R     :1
	 * 22-8 : OCR      :16
	 * 7-0  : reserved :8
	 */
	uint8_t S18R :1;
	uint8_t res1 :3;
	uint8_t xpc  :1;
	uint8_t fb   :1;
	uint8_t hcs  :1;
	uint8_t busy :1;

	uint16_t OCR;
	uint8_t res2;
};

void sd_command(uint8_t cmd_ix, uint32_t arg)
{
	/* cmd packet:
	 * 0 | 1 | [cmd_index:6] | [content:32] | [crc: 7] | 1
	 * 48 bit len, 6 byte len
	 * 38 user bits, 4.75 user bytes
	 */

	/* response packets:
	 * 0 | 0 | [content] | 1 :: 48 bit / 6 byte
	 * 0 | 0 | [content={cid,csd}] | crc | 1 :: 136 bin / 17 byte
	 */

}


