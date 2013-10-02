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


/* Stated SPI limitations:
 * - fixed block size (TODO: check hw behavior)
 * - cannot use CMDs defined after 2.00 (TODO: check hw behavior)
 * - ???
 */

#define SD_SPI_BLOCK_LEN 512

/* SD initialization flow
 *
 * ==> Not for SPI <==
 *     -----------
 *
 * Power On
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

/*
 * [Idle state] valid commands:
 * RESET
 * SEND_IF_COND
 * SEND_OP_COND
 * READ_OCR
 * CRC_ON_OFF
 */

/* Command Format
 * byte 1                     | bytes 2-5       | byte 6
 * '0' '1' <command, 6 bits> | 4 byte argument | <CRC, 7 bits> '1'
 */

/* CMD0: Only for SD, not MMC */
#define GO_IDLE_STATE 0x40 // [basic]
// 0x40, 0, 0, 0, 0, 0x95

/* CMD1: funky rules durring reset, see standard */
#define SEND_OP_COND 0x41 // [basic]

// CMD5 (optional) [IO mode]
// SWITCH_FUNC CMD6 [switch]

/* CMD8: >= 2.00 only */
#define SEND_IF_COND  0x48 // [basic]

// SEND_CSD          CMD9 [basic]
// SEND_CID          CMD10 [basic]

/* CMD12: [basic] */
#define STOP_TRANSMISSION (0x40|12)

// SEND_STATUS       CMD13 [basic]

// <reserved, not specified> CMD14
// <not allowed in SPI mode> CMD15

/* CMD16: [block read, block write, lock card] */
#define SET_BLOCKLEN (0x40|16)
/* CMD17: [block read] */
#define READ_SINGLE_BLOCK (0x40|17)
/* CMD18: [block read] */
#define READ_MULTIPLE_BLOCK (0x40|18)


// <reserved, not specified> CMD19
// <not allowed in SPI mode> CMD20
// <reserved, not specified> CMD21,22,23

// CMD24 [block write]
#define WRITE_SINGLE_BLOCK (0x40|24)
// CMD25 [block write]
#define WRITE_MULTIPLE_BLOCK (0x40|25)

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

/* CMD55: */
#define APP_CMD (0x40|55)

/* CMD58: */
#define READ_OCR (0x40|58)

/* CMD59: */
#define CRC_ON_OFF (0x40|59)


/** Responses **/

/* R1 */
#define R1_START_BIT 7
#define R1_PARAMETER_ERROR 6
#define R1_ADDRESS_ERROR 5
#define R1_ERASE_SEQUENCE_ERROR 4
#define R1_CRC_ERROR 3
#define R1_ILLEGAL_CMD 2
#define R1_ERASE_RESET 1
#define R1_IN_IDLE_STATE 0

/* R2 - R1 followed by an additional byte */
#define R2_OUT_OF_RANGE 7
#define R2_CSD_OVERWRITE 7
#define R2_ERASE_PARAMETER 6
#define R2_WRITE_PROTECT_VIOLATION 5
#define R2_CARD_ECC_FAILED 4
#define R2_CARD_CONTROLLER_ERROR 3
#define R2_UNSPECIFIED_ERROR 2
#define R2_WRITE_PROTECT_ERASE_SKIP 1
#define R2_LOCK_UNLOCK_FAILED 1
#define R2_CARD_LOCKED 0

/* R3 - R1 followed by 4 bytes of the operating condition register (MSB
 * first) */

#endif /* SD_H_ */
