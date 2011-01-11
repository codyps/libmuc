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

#ifndef SPISLAVE_H
#define SPISLAVE_H

#include <stdint.h>

void spi_slave_init(void);
uint8_t spi_slave_put(uint8_t *b, uint8_t n);  // Put bytes to output.
uint8_t spi_slave_get(uint8_t *b, uint8_t n);  // Get bytes received.


#endif

