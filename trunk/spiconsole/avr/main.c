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

int 
main(void)
{
  uint8_t values[4] = {0, 0, 0, 0};

  // Enable DO as output, set initial value low.
  DDRA  = _BV(5);
  PORTA = 0;

  spislave_init();

  sei();

  while (1)
  {
    uint8_t ix;
    uint8_t n;

    // Process incoming values from SPI master, 
    // If a lower case ascii was received, write it back as upper case.
    n = spislave_get(values, 4);

    for (ix = 0; ix < n; ++ix)
      if (values[ix] >= 'a' && values[ix] <= 'z')
        values[ix] -= ('a' - 'A');

      else if (values[ix] == ' ')
        values[ix] = ' ';

      else
        values[ix] = '\0';

    // Write values back out.
    spislave_put(values, n);
  }

  return 0;
}

