#ifndef _SPI_IO_H_
#define _SPI_IO_H_

#include <stdio.h>
#include <stdint.h>

#include "spi_io_conf.h"

void spi_io_init(void);

// state variables.
extern volatile uint8_t spi_io_rx_nl; // new line counter

// non-standard (fast) io
void spi_putchar(uint8_t ch);
void spi_puts(const char * string);
void spi_o_puts(const char * string); // overwriting puts
void spi_puth(uint8_t hex); 	  // put hex (uppercase, no prefix added)
void spi_puth2(uint16_t hex);	  // ^^, 2 bytes.
int spi_getchar(void);

// standard io
//#define SPI_IO_STANDARD // 106B flash / 16B ram
#ifdef SPI_IO_STANDARD
//static int spi_getc(FILE * stream);
//static int spi_putc(char c, FILE * stream);
extern FILE * spi_io;
#endif

#endif
