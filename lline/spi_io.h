#ifndef _SPI_IO_H_
#define _SPI_IO_H_

#include <stdio.h>

void spi_io_init(void);

// non-standard (fast) io
void spi_puts(char * string);
void spi_o_puts(char * string); // overwriting puts
void spi_puth(uint8_t hex); 	  // put hex (uppercase, no prefix added)

// standard io
int spi_getc(FILE * stream);
int spi_putc(char c, FILE * stream);
FILE * spi_io;

#endif
