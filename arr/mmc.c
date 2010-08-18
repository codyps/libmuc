/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in SPI mode) control module  (C)ChaN, 2007           */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), disk_timerproc() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/

#include <avr/io.h>
#include "diskio.h"

/* Definitions for MMC/SDC command */
#define CMD0	(0)             /* GO_IDLE_STATE */
#define CMD1	(1)             /* SEND_OP_COND (MMC) */
#define ACMD41	(0x80+41)       /* SEND_OP_COND (SDC) */
#define CMD8	(8)             /* SEND_IF_COND */
#define CMD9	(9)             /* SEND_CSD */
#define CMD10	(10)            /* SEND_CID */
#define CMD12	(12)            /* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)       /* SD_STATUS (SDC) */
#define CMD16	(16)            /* SET_BLOCKLEN */
#define CMD17	(17)            /* READ_SINGLE_BLOCK */
#define CMD18	(18)            /* READ_MULTIPLE_BLOCK */
#define CMD23	(23)            /* SET_BLOCK_COUNT (MMC) */
#define ACMD23	(0x80+23)       /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)            /* WRITE_BLOCK */
#define CMD25	(25)            /* WRITE_MULTIPLE_BLOCK */
#define CMD55	(55)            /* APP_CMD */
#define CMD58	(58)            /* READ_OCR */

/* Port Controls  (Platform dependent) */
#define CS_LOW()  PORTB &= ~1 /* MMC CS = L */
#define CS_HIGH() PORTB |= 1  /* MMC CS = H */

#define SOCKPORT PINB /* Socket contact port */
#define SOCKWP   0x20 /* Write protect switch (PB5) */
#define SOCKINS  0x10 /* Card detect switch (PB4) */

#define FCLK_SLOW() SPCR = 0x52	/* Set slow clock (100k-400k) */
#define FCLK_FAST() SPCR = 0x50	/* Set fast clock (depends on the CSD) */

static volatile DSTATUS Stat = STA_NOINIT; /* Disk status */
static volatile uint8_t Timer1, Timer2;    /* 100Hz decrement timer */
static uint8_t CardType;                   /* Card type flags */

/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/
#define xmit_spi(dat) SPDR=(dat); loop_until_bit_is_set(SPSR,SPIF)

/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/
static uint8_t rcvr_spi(void)
{
	SPDR = 0xFF;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

/* Alternative macro to receive data fast */
#define rcvr_spi_m(dst)	SPDR=0xFF; loop_until_bit_is_set(SPSR,SPIF); *(dst)=SPDR

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static int wait_ready(void)
{				/* 1:OK, 0:Timeout */
	Timer2 = 50;		/* Wait for ready in timeout of 500ms */
	rcvr_spi();
	do {
		if (rcvr_spi() == 0xFF)
			return 1;
	} while (Timer2);

	return 0;
}

static void deselect(void)
{
	/* Deselect the card and release SPI bus */
	CS_HIGH();
	rcvr_spi();
}

static int select(void)
{
	/* Select the card and wait for ready */
	/* 1:Successful, 0:Timeout */
	CS_LOW();
	if (!wait_ready()) {
		deselect();
		return 0;
	}
	return 1;
}

/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

static void power_on(void)
{
	PORTE &= ~0x80;		/* Socket power ON */
	for (Timer1 = 3; Timer1;) ;	/* Wait for 30ms */
	PORTB = 0b10110101;	/* Enable drivers */
	DDRB = 0b11000111;
	SPCR = 0x52;		/* Enable SPI function in mode 0 */
	SPSR = 0x01;		/* 2x mode */
}

static void power_off(void)
{
	select();		/* Wait for card ready */
	deselect();

	SPCR = 0;		/* Disable SPI function */
	DDRB = 0b11000000;	/* Disable drivers */
	PORTB = 0b10110000;
	PORTE |= 0x80;		/* Socket power OFF */
	Stat |= STA_NOINIT;	/* Set STA_NOINIT */
}

static int chk_power(void)
{				/* Socket power state: 0=off, 1=on */
	return (PORTE & 0x80) ? 0 : 1;
}

/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static int rcvr_datablock(uint8_t * buff, /* Data buffer to store received data */
			  UINT btr        /* Byte count (must be multiple of 4) */
    )
{
	uint8_t token;

	Timer1 = 20;
	do {			/* Wait for data packet in timeout of 200ms */
		token = rcvr_spi();
	} while ((token == 0xFF) && Timer1);
	if (token != 0xFE)
		return 0;	/* If not valid data token, retutn with error */

	do {			/* Receive the data block into buffer */
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
	} while (btr -= 4);
	rcvr_spi();		/* Discard CRC */
	rcvr_spi();

	return 1;		/* Return with success */
}

/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
static
int xmit_datablock(const uint8_t * buff, /* 512 byte data block to be transmitted */
		   uint8_t token	 /* Data/Stop token */
    )
{
	uint8_t resp, wc;

	if (!wait_ready())
		return 0;

	xmit_spi(token);	/* Xmit data token */
	if (token != 0xFD) {	/* Is data token */
		wc = 0;
		do {		/* Xmit the 512 byte data block to MMC */
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (--wc);
		xmit_spi(0xFF);	/* CRC (Dummy) */
		xmit_spi(0xFF);
		resp = rcvr_spi();	/* Reveive data response */
		if ((resp & 0x1F) != 0x05)	/* If not accepted, return with error */
			return 0;
	}

	return 1;
}
#endif				/* _READONLY */

/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/
/* Returns R1 resp (bit7==1:Send failed) */
static
uint8_t send_cmd(uint8_t cmd, uint32_t arg)
{
	uint8_t n, res;

	if (cmd & 0x80) { /* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1)
			return res;
	}

	/* Select the card and wait for ready */
	deselect();
	if (!select())
		return 0xFF;

	/* Send command packet */
	xmit_spi(0x40 | cmd);	/* Start + Command index */
	xmit_spi((uint8_t) (arg >> 24));	/* Argument[31..24] */
	xmit_spi((uint8_t) (arg >> 16));	/* Argument[23..16] */
	xmit_spi((uint8_t) (arg >> 8));	/* Argument[15..8] */
	xmit_spi((uint8_t) arg);	/* Argument[7..0] */
	n = 0x01;		/* Dummy CRC + Stop */
	if (cmd == CMD0)
		n = 0x95;	/* Valid CRC for CMD0(0) */
	if (cmd == CMD8)
		n = 0x87;	/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive command response */
	if (cmd == CMD12)
		rcvr_spi();	/* Skip a stuff byte when stop reading */
	n = 10;	/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while ((res & 0x80) && --n);

	return res; /* Return with the response value */
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(uint8_t drv /* Physical drive nmuber (0) */)
{
	uint8_t n, cmd, ty, ocr[4];

	if (drv)
		return STA_NOINIT; /* Supports only single drive */
	if (Stat & STA_NODISK)
		return Stat;       /* No card in the socket */

	power_on();
	FCLK_SLOW();
	for (n = 10; n; n--)
		rcvr_spi();	/* 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) { /* Enter Idle state */
		Timer1 = 100; /* Initialization timeout of 1000 msec */
		if (send_cmd(CMD8, 0x1AA) == 1) { /* SDv2? */
			for (n = 0; n < 4; n++)
				/* Get trailing return value of R7 resp */
				ocr[n] = rcvr_spi();

			/* The card can work at vdd range of 2.7-3.6V */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
				/* Wait for exit idle state (ACMD41 with HCS bit) */
				while (Timer1 && send_cmd(ACMD41, 1UL << 30))
					;

				/* Check CCS bit in the OCR */
				if (Timer1 && send_cmd(CMD58, 0) == 0) {
					for (n = 0; n < 4; n++)
						ocr[n] = rcvr_spi();
					/* SDv2 */
					ty = (ocr[0] & 0x40) ?
						CT_SD2 | CT_BLOCK : CT_SD2;
				}
			}
		} else {	/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) {
				ty = CT_SD1;
				cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC;
				cmd = CMD1;	/* MMCv3 */
			}

			/* Wait for leaving idle state */
			while (Timer1 && send_cmd(cmd, 0))
				;

			/* Set R/W block length to 512 */
			if (!Timer1 || send_cmd(CMD16, 512) != 0)
				ty = 0;
		}
	}
	CardType = ty;
	deselect();

	if (ty) { /* Initialization succeded */
		Stat &= ~STA_NOINIT; /* Clear STA_NOINIT */
		FCLK_FAST();
	} else { /* Initialization failed */
		power_off();
	}

	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(uint8_t drv	/* Physical drive nmuber (0) */)
{
	if (drv)
		return STA_NOINIT;	/* Supports only single drive */
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(uint8_t drv,    /* Physical drive nmuber (0) */
		  uint8_t * buff, /* data buffer to store read data */
		  uint32_t sector,/* Start sector number (LBA) */
		  uint8_t count	  /* Sector count (1..255) */
    )
{
	if (drv || !count)
		return RES_PARERR;
	if (Stat & STA_NOINIT)
		return RES_NOTRDY;

	if (!(CardType & CT_BLOCK))
		sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) { /* Single block read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
		    &&rcvr_datablock(buff, 512))
			count = 0;
	} else {		/* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512))
					break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);	/* STOP_TRANSMISSION */
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write(uint8_t drv,          /* Physical drive nmuber (0) */
		   const uint8_t * buff, /* Pointer to the data to be written */
		   uint32_t sector,      /* Start sector number (LBA) */
		   uint8_t count         /* Sector count (1..255) */
    )
{
	if (drv || !count)
		return RES_PARERR;
	if (Stat & STA_NOINIT)
		return RES_NOTRDY;
	if (Stat & STA_PROTECT)
		return RES_WRPRT;

	if (!(CardType & CT_BLOCK))
		sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
		    &&xmit_datablock(buff, 0xFE))
			count = 0;
	} else {		/* Multiple block write */
		if (CardType & CT_SDC)
			send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC))
					break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}
#endif				/* _READONLY == 0 */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL != 0
DRESULT disk_ioctl(uint8_t drv, /* Physical drive nmuber (0) */
		   uint8_t ctrl, /* Control code */
		   void *buff    /* Buffer to send/receive control data */
    )
{
	DRESULT res;
	uint8_t n, csd[16], *ptr = buff;
	uint16_t csize;

	if (drv)
		return RES_PARERR;

	res = RES_ERROR;

	if (ctrl == CTRL_POWER) {
		switch (*ptr) {
		case 0:	/* Sub control code == 0 (POWER_OFF) */
			if (chk_power())
				power_off();
			res = RES_OK;
			break;
		case 1:	/* Sub control code == 1 (POWER_ON) */
			power_on();
			res = RES_OK;
			break;
		case 2:	/* Sub control code == 2 (POWER_GET) */
			*(ptr + 1) = (uint8_t) chk_power();
			res = RES_OK;
			break;
		default:
			res = RES_PARERR;
		}
	} else {
		if (Stat & STA_NOINIT)
			return RES_NOTRDY;

		switch (ctrl) {
		case CTRL_SYNC:	/* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
			if (select()) {
				deselect();
				res = RES_OK;
			}
			break;

		case GET_SECTOR_COUNT:	/* Get number of sectors on the disk (uint32_t) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
					csize =
					    csd[9] + ((uint16_t) csd[8] << 8) +
					    1;
					*(uint32_t *) buff =
					    (uint32_t) csize << 10;
				} else {	/* SDC ver 1.XX or MMC */
					n = (csd[5] & 15) +
					    ((csd[10] & 128) >> 7) +
					    ((csd[9] & 3) << 1) + 2;
					csize =
					    (csd[8] >> 6) +
					    ((uint16_t) csd[7] << 2) +
					    ((uint16_t) (csd[6] & 3) << 10) + 1;
					*(uint32_t *) buff =
					    (uint32_t) csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE:
			/* Get R/W sector size (uint16_t) */
			*(uint16_t *) buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE:
			/* Get erase block size in unit of sector (uint32_t) */
			if (CardType & CT_SD2) {	/* SDv2? */
				if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
					rcvr_spi();
					if (rcvr_datablock(csd, 16)) {
						/* Read partial block */
						for (n = 64 - 16; n; n--)
							/* Purge trailing data */
							rcvr_spi();
						*(uint32_t *) buff =
						    16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {/* SDv1 or MMCv3 */
				if ((send_cmd(CMD9, 0) == 0)
				    && rcvr_datablock(csd, 16)) { /* Read CSD */
					if (CardType & CT_SD1) {	/* SDv1 */
						*(uint32_t *) buff =
						    (((csd[10] & 63) << 1) +
						     ((uint16_t) (csd[11] & 128)
						      >> 7) +
						     1) << ((csd[13] >> 6) - 1);
					} else {	/* MMCv3 */
						*(uint32_t *) buff =
						    ((uint16_t)
						     ((csd[10] & 124) >> 2) +
						     1) *
						    (((csd[11] & 3) << 3) +
						     ((csd[11] & 224) >> 5) +
						     1);
					}
					res = RES_OK;
				}
			}
			break;

		case MMC_GET_TYPE:
			/* Get card type flags (1 byte) */
			*ptr = CardType;
			res = RES_OK;
			break;

		case MMC_GET_CSD:
			/* Receive CSD as a data block (16 bytes) */
			if (send_cmd(CMD9, 0) == 0	/* READ_CSD */
			    && rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_CID:
			/* Receive CID as a data block (16 bytes) */
			if (send_cmd(CMD10, 0) == 0	/* READ_CID */
			    && rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_OCR:
			/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
				for (n = 4; n; n--)
					*ptr++ = rcvr_spi();
				res = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT:
			/* Receive SD statsu as a data block (64 bytes) */
			if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
				rcvr_spi();
				if (rcvr_datablock(ptr, 64))
					res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
		}

		deselect();
	}

	return res;
}
#endif				/* _USE_IOCTL != 0 */

/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

void disk_timerproc(void)
{
	static uint8_t pv;
	uint8_t n, s;

	n = Timer1;		/* 100Hz decrement timer */
	if (n)
		Timer1 = --n;
	n = Timer2;
	if (n)
		Timer2 = --n;

	n = pv;
	pv = SOCKPORT & (SOCKWP | SOCKINS);	/* Sample socket switch */

	if (n == pv) {		/* Have contacts stabled? */
		s = Stat;

		if (pv & SOCKWP)	/* WP is H (write protected) */
			s |= STA_PROTECT;
		else		/* WP is L (write enabled) */
			s &= ~STA_PROTECT;

		if (pv & SOCKINS)	/* INS = H (Socket empty) */
			s |= (STA_NODISK | STA_NOINIT);
		else		/* INS = L (Card inserted) */
			s &= ~STA_NODISK;

		Stat = s;
	}
}
