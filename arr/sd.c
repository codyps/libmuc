#include <stdio.h>
#include <avr/io.h>

#include "spi_master.h"
#include "sd_conf.h"

static FILE sd_files[SD_FILE_CT];
static uint8_t sd_files_pos;

#define sd_cs_release() SD_CS_PORT |= (1 << SD_CS_IX)
#define sd_cs_raise()   SD_CS_PORT &= ~(1 << SD_CS_IX)

/* MSB first.
 * CRC = 16 bit CCITT
 */

void sd_init(void)
{

	spi_master_init();

	/* SD CS = output */
	DDR(SD_CS_PORT) |= (1 << SD_CS_IX);

	sd_cs_release();
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


