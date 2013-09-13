#ifndef SD_H_
#define SD_H_

#define SD_CID_SZ (128/8)	// card identification number
#define SD_RCA_SZ (16/8)	// relative card address
#define SD_DSR_SZ (16/8)	// driver stage register
#define SD_CSD_SZ (128/8)	// card specific data
#define SD_SCR_SZ (64/8)	// SD configuration register
#define SD_OCR_SZ (32/8)	// operation conditions register
#define SD_SSR_SZ (512/8)	// SD status
#define SD_CSR_SZ (32/8)	// card status

/* Power On
 *  |          CMD0 (from almost all states)
 *  |   /------/
 *  v  v
 *  [Idle state (idle)] ----> CMD0 with CS Asserted ("0") -> SPI MODE
 *  |  ^  |
 *  |  |  \--> CMD8 (Mandatory for phys spec v2.00)
 *  |  |       |
 *  |  \-------/
 *  v
 *  ACMD41 -> [Inactive state] (cards with non compatable voltage range)
 *  | |
 *  | \--> No responce (not a valid command), Must be an MMC card, Begin MMC init at CMD1.
 *  |
 *  v
 *  [Ready State (ready)]
 *  |  |
 *  |  v
 *  |  CMD11 (voltage switch sequence, determined by ACMD41)
 *  |  |
 *  v  v
 *  CMD2
 *  |
 *  v
 *  [Identification state (ident)]
 *
 */

/* [Idle state] valid commands:
 * RESET
 * SEND_IF_COND
 * SEND_OP_COND
 * READ_OCR
 * CRC_ON_OFF
 */



/* Only for SD, not MMC */
#define GO_IDLE_STATE CMD0 // [basic]
// 0x40, 0, 0, 0, 0, 0x95

/* funky rules durring reset, see standard */
#define SEND_OP_COND CMD1 // [basic]

// CMD5 (optional) [IO mode]
// SWITCH_FUNC CMD6 [switch]

#define SEND_IF_COND  CMD8 // (>=2.00) [basic]

// SEND_CSD          CMD9 [basic]
// SEND_CID          CMD10 [basic]
// STOP_TRANSMISSION CMD12 [basic]
// SEND_STATUS       CMD13 [basic]

// <reserved, not specified> CMD14
// <not allowed in SPI mode> CMD15

// CMD16 [block read, block write, lock card]
#define SET_BLOCKLEN CMD16
// CMD17 [block read]
#define READ_SINGLE_BLOCK CMD17
// CMD18 [block read]
#define READ_MULTIPLE_BLOCK CMD18

// <reserved, not specified> CMD19
// <not allowed in SPI mode> CMD20
// <reserved, not specified> CMD21,22,23

// CMD24 [block write]
#define WRITE_SINGLE_BLOCK CMD24
// CMD25 [block write]
#define WRITE_MULTIPLE_BLOCK CMD25

// PROGRAM_CSD	CMD27 [block write]
//
// SET_WRITE_PROT  CMD28 (optional) [write protection]
// CLR_WRITE_PROT  CMD29 (optional) [write protection]
// SEND_WRITE_PROT CMD30 (optional) [write protection]
//
// <reserved, not specified> CMD31
//
// ERASE_WR_BLK_START_ADDR	CMD32 [erase]
// ERASE_WR_BLK_END_ADDR	CMD33 [erase]
// <special meanings>		CMD34-37 (optional) [switch]
// ERASE			CMD38 [erase]
//
// LOCK_UNLOCK			CMD42 (special) [lock card]
// <reserved for CMD6>		CMD50 (optional) [switch]
// <reserved for IO mode>	CMD52 (optional) [i/o mode]
// <reserved for IO mode>	CMD53 (optional) [i/o mode]
// APP_CMD			CMD55 [application specific]
// GEN_CMD			CMD56 [application specific]
// <reserved for CMD6>		CMD57 (optional) [switch]
// READ_OCR			CMD58 [basic]
// CRC_ON_OFF			CMD59 [basic]
//
// CMD60-63 are reserved for manuf.
//
// ACMD13 [app specific]
// ACMD22 [app specific]
// ACMD23 [app specific]
// ACMD41 [app specific]
// ACMD42 [app specific]
// ACMD51 [app specific]

#define SD_SEND_OP_COND ACMD41
#define APP_CMD       CMD55

#define READ_OCR CMD58
#define CRC_ON_OFF CMD59

#define SET_BLOCKLEN CMD16
#define STOP_TRANS CMD12

#endif /* SD_H_ */
