#ifndef _SPI_IO_H_
#define _SPI_IO_H_

#include <stdio.h>

void spi_io_init(void);
void spi_puts(char * string);
void spi_o_puts(char * string); // overwriting puts

int spi_getc(FILE * stream);
int spi_putc(int c, FILE * stream);

FILE * io_spi;

#endif
