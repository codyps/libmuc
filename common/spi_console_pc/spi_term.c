/*
 * spiconsole - A simple interface to usbtinyisp to act as a 'console' 
 *              using SPI
 * Copyright (C) 2008 Matt Anderson, adapted from usbtiny.c from avrdude
 *
 * avrdude - A Downloader/Uploader for AVR device programmers
 * Copyright (C) 2007 Dick Streefland, adapted for 5.4 by Limor Fried
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
#include <stdio.h>
#include <unistd.h>

#include <string.h>
#include <ctype.h>

#include "usbtiny.h"
#include "usbtiny_def.h"

/* Why not. */
const char *progname;

void do_printf_ui(void) {
  uint8_t cont = 1;	

	while(cont) {
		uint8_t input[4] = { 0, 0, 0, 0 };
		uint8_t output[4];
		

    /*
		{
		uint8_t c;
		uint8_t i = 0;
		while((c = getch()) && i < 4)
		{
			if (c == '\003')
				cont = 0;

			input[i] = c;
			i++;
		}
		}
    */

		usbtiny_spi(input,output);
		
		for(uint8_t i = 0; i < sizeof(input); i++ ) {
			if (output[i] && isascii(output[i]))
				putchar(output[i]);
		}
		usleep(1000);
	}

	return;
}

int main(int argc, char **argv)
{
  // XXX: bitclock always default for now.
  // any faster than this with 1Mhz avr clock and bit errors are too likely.
  double bitclock = 0.0000500;  

  progname = argv[0];

  /* Find and open usbtiny. */
  usbtiny_open();

  // Check for bit-clock and tell the usbtiny to adjust itself
  usbtiny_set_sck_period(bitclock);


  // Let the device wake up.
  usleep(50000);

  //do_ncurses_ui();
  do_printf_ui();

  usb_control(USBTINY_POWERDOWN, 0, 0);      // Send USB control command to device
  usbtiny_close();

  return 0;
}

