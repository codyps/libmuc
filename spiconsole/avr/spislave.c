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

#include "spislave.h"

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
  USISR = _BV(USIOIF);

  // If byte in USIDR is non zero and space is available in the 
  // rx queue, read byte from USIDR and place in rx queue.
  if (USIDR && rx.len < NUM_BUFFER_BYTES)
  {
    rx.buf[(rx.cur + rx.len) % NUM_BUFFER_BYTES] = USIDR;
    ++rx.len;
  }

  // If bytes are pending in the tx queue place in USIDR.
  if (tx.len)
  {
    USIDR = tx.buf[tx.cur];

    tx.cur = (tx.cur + 1) % NUM_BUFFER_BYTES;
    --tx.len;
  }

  // Otherwise clear USIDR.
  else
    USIDR = 0;
    
}

void
spislave_init(void)
{
  // Enable three wire mode, external positive edge clock,
  // Interrupt on counter overflow
  USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USIOIE);
}

uint8_t 
spislave_put(uint8_t *b, uint8_t n)
{
  uint8_t i;

  i = 0;

  cli();

  while ((i < n) && (tx.len + i < NUM_BUFFER_BYTES))
    tx.buf[(tx.cur + tx.len + i) % NUM_BUFFER_BYTES] = b[i++];

  tx.len += i;

  sei();

  // Return number of bytes added to tx buffer.
  return i;
}

uint8_t 
spislave_get(uint8_t *b, uint8_t n)
{
  uint8_t i;

  i = 0;

  cli();

  while (rx.len && i < n)
  {
    b[i] = rx.buf[rx.cur];
    rx.cur = (rx.cur + 1) % NUM_BUFFER_BYTES;
    --rx.len;
    ++i;
  }

  sei();

  return i;
}

