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

#include "spi_slave.h"

#define NUM_BUFFER_BYTES 32

struct buffer_type
{
  uint8_t buf[NUM_BUFFER_BYTES];
  uint8_t cur;
  uint8_t len;
};

static struct buffer_type tx, rx;
 
ISR(SIG_USI_OVERFLOW)
{
  // Clear counter overflow interupt flag and reset 4-byte counter.
  USISR = (1<<USIOIF);

  // If byte in USIDR is non zero and space is available in the 
  // rx queue, read byte from USIDR and place in rx queue.
  if (USIDR && rx.len < NUM_BUFFER_BYTES)
  {
    uint8_t pos = rx.cur + rx.len;
    while (pos >= NUM_BUFFER_BYTES) {
      pos -= NUM_BUFFER_BYTES;
    }
    rx.buf[pos] = USIDR;
    ++rx.len;
  }

  // If bytes are pending in the tx queue place in USIDR.
  if (tx.len)
  {
    USIDR = tx.buf[tx.cur];

    uint8_t cur = tx.cur+1;
    while (cur >= NUM_BUFFER_BYTES) {
      cur -= NUM_BUFFER_BYTES;
    }
    tx.cur = cur;
    --tx.len;
  }

  // Otherwise clear USIDR.
  else
    USIDR = 0;
    
}

void
spi_slave_init(void)
{
  power_usi_enable();
  // Enable three wire mode, external positive edge clock,
  // Interrupt on counter overflow
  DDRB&=~(1<<0)|(1<<2); // inputs
  DDRB|= (1<<1);        // outputs
  PORTB&=~(1<<0)|(1<<2); // disable pullups
  PORTB&=~(1<<1);       // start low

  USIDR = 0;    
  USICR = (1<<USIWM0) | (1<<USICS1) | (1<<USIOIE);
}

#include <spi_slave_common.c>
