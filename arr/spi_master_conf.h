#ifndef SPI_CONF_H_
#define SPI_CONF_H_

#define SPI_PORT PORTB
#define SPI_MOSI_IX 3
#define SPI_MISO_IX 4
#define SPI_SCK_IX  5


/* SPI Frequency
SPI2X SPR1 SPR0 SCK Frequency
  0     0    0  fosc/4
  0     0    1  fosc/16
  0     1    0  fosc/64
  0     1    1  fosc/128
  1     0    0  fosc/2
  1     0    1  fosc/8
  1     1    0  fosc/32
  1     1    1  fosc/64
 * SPRx = (2 << SPI2X) | (1 << SPR1) | (0 << SPR0)
 */
#define SPRx 2



#endif
