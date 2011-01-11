/*
 * avrtest - A simple interface to usbtinyisp to act as a 'console' 
 *           using SPI, target software to test for attiny84.
 * Copyright (C) 2008 Matt Anderson.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include "spislave.h"

#define NUM_BUFFER_BYTES 32

struct buffer_type
{
  uint8_t buf[NUM_BUFFER_BYTES];
  uint8_t cur;
  uint8_t len;
};

static volatile struct buffer_type tx, rx;
 
ISR(SIG_SPI)
{  // If byte in USIDR is non zero and space is available in the 
  // rx queue, read byte from USIDR and place in rx queue.
  uint8_t spdr = SPDR;
  if (spdr && rx.len < NUM_BUFFER_BYTES)
  {
    uint8_t pos = rx.cur + rx.len;
    while (pos >= NUM_BUFFER_BYTES) {
      pos -= NUM_BUFFER_BYTES;
    }
    rx.buf[pos] = spdr;
    ++rx.len;
  }

  // If bytes are pending in the tx queue place in USIDR.
  if (tx.len)
  {
    SPDR = tx.buf[tx.cur];

    uint8_t cur = tx.cur+1;
    while (cur >= NUM_BUFFER_BYTES) {
      cur -= NUM_BUFFER_BYTES;
    }
    tx.cur = cur;

    --tx.len;
  }

  // Otherwise clear USIDR.
  else
    SPDR = 0;
    
}

void
spi_slave_init(void)
{
  power_spi_enable();
  // Enable three wire mode, external positive edge clock,
  // Interrupt on counter overflow
  // Interupt, enable, msb first, slave, leading edge on rise, sample on leading edge.
 
	  
  DDRB&=~(1<<5)|(1<<7)|(1<<4); // inputs
  DDRB|= (1<<6);	// outputs
  PORTB&=~(1<<5)|(1<<7)|(1<<4)|(1<<6); // disable pullups & start low
  SPCR = (1<<SPIE) | (1<<SPE) | (1 << DORD) | (0 << MSTR) | (0 << CPOL) | (0 << CPHA);
  SPDR = 0;
}

#include <spi_slave_common.c>
