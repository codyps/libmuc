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
#include <ncurses.h>
#include <ctype.h>

#include "usbtiny.h"
#include "usbtiny_def.h"

/* Why not. */
const char *progname;



static void
do_ncurses_ui(void)
{
  uint8_t running = 1;
  char **disp;

  int row, col;
  int posx = 0;
  int ix;

  // Do ncurses init stuffs.
  initscr();
  raw();
  noecho();
  timeout(0);

  getmaxyx(stdscr, row, col);

  row -= 1;

  disp = (char**) malloc(sizeof(char*) * row);
  
  for (ix = 0; ix < row; ++ix)
  {
    disp[ix] = (char*) malloc(sizeof(char) * (col+1));
    disp[ix][col] = '\0';
  }

  while (running)
  {
    int c;

    unsigned char input[4] = { 0, 0, 0, 0 };
    unsigned char output[4];

    ix = 0;

    while ((c = getch()) && ix < 4)
    {
      if (c == '\003')
        running = 0;

      input[ix] = c;
      ++ix;
    }

    // Exchange 4 bytes with usbtiny.
    usbtiny_spi(input, output);

    // If we received bytes in return, write them out.
    for (ix = 0; ix < 4; ++ix)
    {
      if (output[ix] && isascii(output[ix])) {
		if (output[ix] == '\n') posx=col;
		else
	        sprintf(&disp[row-1][posx++], "%c", output[ix]);
	 }
#if 0
      else
        sprintf(&disp[row-1][posx++], ".");
#endif

      if (posx == col)
      {
        int iy;

        for(iy = 0; iy < row - 1; ++iy)
          strcpy(disp[iy], disp[iy+1]);

        disp[row-1][0] = '\0';
        posx = 0;
      }
    }

    for (ix = 0; ix < row; ++ix)
      mvprintw(ix, 0, disp[ix]);

    clrtoeol();
    refresh();

    usleep(1000);
  }

  clrtoeol();
  refresh();
  endwin();

}

void do_printf_ui(void) {
	
	for(;;) {
		uint8_t input[4] = { 0, 0, 0, 0 };
		uint8_t output[4];
		

		{
		uint8_t c;
		uint8_t i;
		while((c = getchar()) && i < 4)
		{
			if (c == '\003')
				goto end;

			input[i] = c;
			i++;
		}
		}

		usbtiny_spi(input,output);
		
		for(uint8_t i = 0; i < sizeof(input); i++ ) {
			if (output[i] && isascii(output[i]))
				putchar(output[i]);
		}
		usleep(1000);
	}
end:
	return;
}

int main(int argc, char **argv)
{
  // XXX: bitclock always default for now.
  // any faster than this with 1Mhz avr clock and bit errors are too likely.
  double bitclock = 0.0002200;  

  progname = argv[0];

  /* Find and open usbtiny. */
  usbtiny_open();

  // Check for bit-clock and tell the usbtiny to adjust itself
  usbtiny_set_sck_period(bitclock);


  // Let the device wake up.
  usleep(50000);

  do_ncurses_ui();
  //do_printf_ui();

  usb_control(USBTINY_POWERDOWN, 0, 0);      // Send USB control command to device
  usbtiny_close();

  return 0;
}

