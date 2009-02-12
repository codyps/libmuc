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

#include <usb.h>

#include <ncurses.h>
#include <ctype.h>

#include "usbtiny.h"


static usb_dev_handle *usb_handle;
static int             sck_period;
static int             chunk_size;

/* Why not. */
const char *progname;


/* Find a device with the correct VID/PID match for USBtiny */
static int
usbtiny_open(void)
{
  struct usb_bus      *busses, *bus;
  struct usb_device   *dev = 0;

  usb_init();                    // initialize the libusb system
  usb_find_busses();             // have libusb scan all the usb busses available
  usb_find_devices();            // have libusb scan all the usb devices available

  busses     = usb_get_busses();
  usb_handle = NULL;

  // now we iterate through all the busses and devices
  for (bus = busses; bus; bus = bus->next) 
  {
    for	(dev = bus->devices; dev; dev = dev->next) 
    {
      if (dev->descriptor.idVendor == USBTINY_VENDOR && 
          dev->descriptor.idProduct == USBTINY_PRODUCT) 
      {   // found match?

        usb_handle = usb_open(dev);           // attempt to connect to device

        // wrong permissions or something?
        if (!usb_handle) {
          fprintf(stderr, "%s: Warning: cannot open USB device: %s\n",
              progname, usb_strerror());
          continue;
        }
      }
    }
  }

  if (!usb_handle) 
  {
    fprintf( stderr, "%s: Error: Could not find USBtiny device (0x%x/0x%x)\n",
	     progname, USBTINY_VENDOR, USBTINY_PRODUCT );
    exit(1);
  }

  return 0;                  // If we got here, we must have found a good USB device
}

static void
usbtiny_close(void)
{
  usb_close(usb_handle);
  usb_handle = NULL;
}

// Wrapper for simple usb_control_msg messages
static int 
usb_control(unsigned int requestid, unsigned int val, unsigned int index)
{
  int nbytes;

  nbytes = usb_control_msg( usb_handle,
      USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
      requestid,
      val, index,           // 2 bytes each of data
      NULL, 0,              // no data buffer in control messge
      USB_TIMEOUT );        // default timeout

  if (nbytes < 0)
  {
    fprintf(stderr, "%s: error: usbtiny_transmit: %s\n", 
        progname, usb_strerror());
    exit(1);
  }

  return nbytes;
}

// Wrapper for simple usb_control_msg messages to receive data from programmer
static int usb_in(unsigned int requestid, unsigned int val, unsigned int index,
    unsigned char* buffer, int buflen, int bitclk )
{
  int nbytes;
  int timeout;

  // calculate the amout of time we expect the process to take by
  // figuring the bit-clock time and buffer size and adding to the standard USB timeout.
  timeout = USB_TIMEOUT + (buflen * bitclk) / 1000;

  nbytes = usb_control_msg( usb_handle,
      USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
      requestid,
      val, index,
      (char *)buffer, buflen,
      timeout);

  if (nbytes != buflen) {
    fprintf(stderr, "%s: error: usbtiny_receive: %s (expected %d, got %d)\n",
        progname, usb_strerror(), buflen, nbytes);
    exit(1);
  }

  return nbytes;
}

/* A simple calculator function determines the maximum size of data we can
   shove through a USB connection without getting errors */
static void
usbtiny_set_chunk_size(int period)
{
  chunk_size = CHUNK_SIZE;       // start with the maximum (default)
  while	(chunk_size > 8 && period > 16) 
  {
    // Reduce the chunk size for a slow SCK to reduce
    // the maximum time of a single USB transfer.
    chunk_size >>= 1;
    period >>= 1;
  }
}


/* Given a SCK bit-clock speed (in useconds) we verify its an OK speed and tell the
   USBtiny to update itself to the new frequency */
static int 
usbtiny_set_sck_period(double v)
{
  sck_period = (int)(v * 1e6 + 0.5);   // convert from us to 'int', the 0.5 is for rounding up

  // Make sure its not 0, as that will confuse the usbtiny
  if (sck_period < SCK_MIN)
    sck_period = SCK_MIN;

  // We can't go slower, due to the byte-size of the clock variable
  if (sck_period > SCK_MAX)
    sck_period = SCK_MAX;

  printf( "%s: Setting SCK period to %d usec\n", progname, sck_period );

  // send the command to the usbtiny device.
  usb_control(USBTINY_POWERUP, sck_period, RESET_HIGH);

  // with the new speed, we'll have to update how much data we send per usb transfer
  usbtiny_set_chunk_size(sck_period);

  return 0;
}

/* Send a 4-byte SPI command to the USBtinyISP for execution */
static int 
usbtiny_spi(unsigned char cmd[4], unsigned char res[4])
{
  int nbytes;

  // Make sure its empty so we don't read previous calls if it fails
  memset(res, '\0', sizeof(res) );

  nbytes = usb_in( USBTINY_SPI,
		   (cmd[1] << 8) | cmd[0],  // convert to 16-bit words
		   (cmd[3] << 8) | cmd[2],  //  "
			res, sizeof(res), 8 * sck_period );

#if 0
  // print out the data we sent and received
  printf( "CMD: [%02x %02x %02x %02x] [%02x %02x %02x %02x]\n",
      cmd[0], cmd[1], cmd[2], cmd[3],
      res[0], res[1], res[2], res[3] );
#endif

  return (nbytes == sizeof(res));     // should have read 4 bytes
}


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
      if (output[ix] && isascii(output[ix]))
        sprintf(&disp[row-1][posx++], "%c", output[ix]);
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

main(int argc, char **argv)
{
  // XXX: bitclock always default for now.
  // any faster than this with 1Mhz avr clock and bit errors are too likely.
  double bitclock = 0.000200;  

  progname = argv[0];

  /* Find and open usbtiny. */
  usbtiny_open();

  // Check for bit-clock and tell the usbtiny to adjust itself
  if (bitclock > 0.0) 
    usbtiny_set_sck_period(bitclock);

  else 
  {
    // -B option not specified: use default
    sck_period = SCK_DEFAULT;

    printf("%s: Using SCK period of %d usec\n", progname, sck_period);

    usb_control(USBTINY_POWERUP, sck_period, RESET_HIGH);
    usbtiny_set_chunk_size(sck_period);
  }

  // Let the device wake up.
  usleep(50000);

  do_ncurses_ui();

  usb_control(USBTINY_POWERDOWN, 0, 0);      // Send USB control command to device
  usbtiny_close();

  return 0;
}

