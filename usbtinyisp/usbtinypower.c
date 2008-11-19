#include "usbtiny.h"
#include <usb.h>

usb_dev_handle *usb_handle;
int sck_period;
int chunk_size;

char * progname;

// Wrapper for simple usb_control_msg messages
static int usb_control (unsigned int requestid, unsigned int val, unsigned int index )
{
  int nbytes;
  nbytes = usb_control_msg(usb_handle,
			    USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			    requestid,
			    val, index,           // 2 bytes each of data
			    NULL, 0,              // no data buffer in control messge
			    USB_TIMEOUT );        // default timeout
  if(nbytes < 0){
    fprintf(stderr, "%s: error: usbtiny_transmit: %s\n", progname, usb_strerror());
    exit(1);
  }

  return nbytes;
}

// Wrapper for simple usb_control_msg messages to receive data from programmer
static int usb_in (unsigned int requestid, unsigned int val, unsigned int index,
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

// Wrapper for simple usb_control_msg messages to send data to programmer
static int usb_out (unsigned int requestid, unsigned int val, unsigned int index,
		    unsigned char* buffer, int buflen, int bitclk )
{
  int nbytes;
  int timeout;

  // calculate the amout of time we expect the process to take by
  // figuring the bit-clock time and buffer size and adding to the standard USB timeout.
  timeout = USB_TIMEOUT + (buflen * bitclk) / 1000;

  nbytes = usb_control_msg( usb_handle,
			    USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			    requ/* A simple calculator function determines the maximum size of data we can
   shove through a USB connection without getting errors */
static void usbtiny_set_chunk_size (PROGRAMMER * pgm, int period)
{
  PDATA(pgm)->chunk_size = CHUNK_SIZE;       // start with the maximum (default)
  while	(PDATA(pgm)->chunk_size > 8 && period > 16) {
    // Reduce the chunk size for a slow SCK to reduce
    // the maximum time of a single USB transfer.
    PDATA(pgm)->chunk_size >>= 1;
    period >>= 1;
  }
}

/* Given a SCK bit-clock speed (in useconds) we verify its an OK speed and tell the
   USBtiny to update itself to the new frequency */
static int usbtiny_set_sck_period (PROGRAMMER *pgm, double v)
{
  PDATA(pgm)->sck_period = (int)(v * 1e6 + 0.5);   // convert from us to 'int', the 0.5 is for rounding up

  // Make sure its not 0, as that will confuse the usbtiny
  if (PDATA(pgm)->sck_period < SCK_MIN)
    PDATA(pgm)->sck_period = SCK_MIN;

  // We can't go slower, due to the byte-size of the clock variable
  if  (PDATA(pgm)->sck_period > SCK_MAX)
    PDATA(pgm)->sck_period = SCK_MAX;

  printf( "%s: Setting SCK period to %d usec\n", progname, PDATA(pgm)->sck_period );

  // send the command to the usbtiny device.
  usb_control(pgm, USBTINY_POWERUP, PDATA(pgm)->sck_period, RESET_LOW);  // MEME: for at90's fix resetstate?

  // with the new speed, we'll have to update how much data we send per usb transfer
  usbtiny_set_chunk_size(pgm, PDATA(pgm)->sck_period);
  return 0;
}


static int usbtiny_initialize (PROGRAMMER *pgm, AVRPART *p )
{
  unsigned char res[4];        // store the response from usbtinyisp

  // Check for bit-clock and tell the usbtiny to adjust itself
  if (pgm->bitclock > 0.0) {
    // -B option specified: convert to valid range for sck_period
    usbtiny_set_sck_period(pgm, pgm->bitclock);
  } else {
    // -B option not specified: use default
    PDATA(pgm)->sck_period = SCK_DEFAULT;
    if	(verbose) {
      printf( "%s: Using SCK period of %d usec\n",
	      progname, PDATA(pgm)->sck_period );
    }
    usb_control(pgm,  USBTINY_POWERUP, PDATA(pgm)->sck_period, RESET_LOW );
    usbtiny_set_chunk_size(pgm, PDATA(pgm)->sck_period);
  }

  // Let the device wake up.
  usleep(50000);

  // Attempt to use the underlying avrdude methods to connect (MEME: is this kosher?)
  if (! usbtiny_avr_op(pgm, p, AVR_OP_PGM_ENABLE, res)) {
    // no response, RESET and try again
    usb_control(pgm, USBTINY_POWERUP, PDATA(pgm)->sck_period, RESET_HIGH);
    usb_control(pgm, USBTINY_POWERUP, PDATA(pgm)->sck_period, RESET_LOW);
    usleep(50000);
    if	( ! usbtiny_avr_op( pgm, p, AVR_OP_PGM_ENABLE, res)) {
      // give up
      return -1;
    }
  }
  return 0;
}

/* Tell the USBtiny to release the output pins, etc */
static void usbtiny_powerdown(PROGRAMMER * pgm)
{
  if (!PDATA(pgm)->usb_handle) {
    return;                 // wasn't connected in the first place
  }
  usb_control(pgm, USBTINY_POWERDOWN, 0, 0);      // Send USB control command to device
}

/* Send a 4-byte SPI command to the USBtinyISP for execution
   This procedure is used by higher-level Avrdude procedures */
static int usbtiny_cmd(PROGRAMMER * pgm, unsigned char cmd[4], unsigned char res[4])
{
  int nbytes;

  // Make sure its empty so we don't read previous calls if it fails
  memset(res, '\0', sizeof(res) );

  nbytes = usb_in( pgm, USBTINY_SPI,
		   (cmd[1] << 8) | cmd[0],  // convert to 16-bit words
		   (cmd[3] << 8) | cmd[2],  //  "
			res, 4, 8 * PDATA(pgm)->sck_period );
  if (verbose > 1) {
    // print out the data we sent and received
    printf( "CMD: [%02x %02x %02x %02x] [%02x %02x %02x %02x]\n",
	    cmd[0], cmd[1], cmd[2], cmd[3],
	    res[0], res[1], res[2], res[3] );
  }
  return ((nbytes == 4) &&      // should have read 4 bytes
	  res[2] == cmd[1]);              // AVR's do a delayed-echo thing
}estid,
			    val, index,
			    (char *)buffer, buflen,
			    timeout);
  if (nbytes != buflen) {
    fprintf(stderr, "%s: error: usbtiny_send: %s (expected %d, got %d)\n",
	    progname, usb_strerror(), buflen, nbytes);
    exit(1);
  }

  return nbytes;
}


static	int usbtiny_open(void)
{
  struct usb_bus      *bus;
  struct usb_device   *dev = 0;

  usb_init();                    // initialize the libusb system
  usb_find_busses();             // have libusb scan all the usb busses available
  usb_find_devices();            // have libusb scan all the usb devices available

  usb_handle = NULL;

  // now we iterate through all the busses and devices
  for ( bus = usb_busses; bus; bus = bus->next ) {
    for	( dev = bus->devices; dev; dev = dev->next ) {
      if (dev->descriptor.idVendor == USBTINY_VENDOR
	  && dev->descriptor.idProduct == USBTINY_PRODUCT ) {   // found match?

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

  if (!usb_handle) {
    fprintf( stderr, "%s: Error: Could not find USBtiny device (0x%x/0x%x)\n",
	     progname, USBTINY_VENDOR, USBTINY_PRODUCT );
    exit(1);
  }

  return 0;                  // If we got here, we must have found a good USB device
}

static	void usbtiny_close (void)
{
  if (!usb_handle) {
    return;                // not a valid handle, bail!
  }
  usb_close(usb_handle);   // ask libusb to clean up
  usb_handle = NULL;
}

/* A simple calculator function determines the maximum size of data we can
   shove through a USB connection without getting errors */
static void usbtiny_set_chunk_size (PROGRAMMER * pgm, int period)
{
  PDATA(pgm)->chunk_size = CHUNK_SIZE;       // start with the maximum (default)
  while	(PDATA(pgm)->chunk_size > 8 && period > 16) {
    // Reduce the chunk size for a slow SCK to reduce
    // the maximum time of a single USB transfer.
    PDATA(pgm)->chunk_size >>= 1;
    period >>= 1;
  }
}

/* Given a SCK bit-clock speed (in useconds) we verify its an OK speed and tell the
   USBtiny to update itself to the new frequency */
static int usbtiny_set_sck_period (PROGRAMMER *pgm, double v)
{
  PDATA(pgm)->sck_period = (int)(v * 1e6 + 0.5);   // convert from us to 'int', the 0.5 is for rounding up

  // Make sure its not 0, as that will confuse the usbtiny
  if (PDATA(pgm)->sck_period < SCK_MIN)
    PDATA(pgm)->sck_period = SCK_MIN;

  // We can't go slower, due to the byte-size of the clock variable
  if  (PDATA(pgm)->sck_period > SCK_MAX)
    PDATA(pgm)->sck_period = SCK_MAX;

  printf( "%s: Setting SCK period to %d usec\n", progname, PDATA(pgm)->sck_period );

  // send the command to the usbtiny device.
  usb_control(pgm, USBTINY_POWERUP, PDATA(pgm)->sck_period, RESET_LOW);  // MEME: for at90's fix resetstate?

  // with the new speed, we'll have to update how much data we send per usb transfer
  usbtiny_set_chunk_size(pgm, PDATA(pgm)->sck_period);
  return 0;
}


static int usbtiny_initialize (AVRPART *p )
{
  unsigned char res[4];        // store the response from usbtinyisp

  sck_period = SCK_DEFAULT;
  if	(verbose) {
    printf( "%s: Using SCK period of %d usec\n", progname, sck_period );
  }
  usb_control(USBTINY_POWERUP, sck_period, RESET_LOW );
  usbtiny_set_chunk_size(sck_period);

  // Let the device wake up.
  usleep(50000);

  // Attempt to use the underlying avrdude methods to connect (MEME: is this kosher?)
  if (! usbtiny_avr_op(pgm, p, AVR_OP_PGM_ENABLE, res)) {
    // no response, RESET and try again
    usb_control(pgm, USBTINY_POWERUP, sck_period, RESET_HIGH);
    usb_control(pgm, USBTINY_POWERUP, sck_period, RESET_LOW);
    usleep(50000);
    if	( ! usbtiny_avr_op( pgm, p, AVR_OP_PGM_ENABLE, res)) {
      // give up
      return -1;
    }
  }
  return 0;
}

/* Tell the USBtiny to release the output pins, etc */
static void usbtiny_powerdown(void)
{
  if (!usb_handle) {
    return;                 // wasn't connected in the first place
  }
  usb_control(USBTINY_POWERDOWN, 0, 0);      // Send USB control command to device
}

/* Send a 4-byte SPI command to the USBtinyISP for execution
   This procedure is used by higher-level Avrdude procedures */
static int usbtiny_cmd(unsigned char cmd[4], unsigned char res[4])
{
  int nbytes;

  // Make sure its empty so we don't read previous calls if it fails
  memset(res, '\0', sizeof(res) );

  nbytes = usb_in( USBTINY_SPI,
		   (cmd[1] << 8) | cmd[0],  // convert to 16-bit words
		   (cmd[3] << 8) | cmd[2],  //  "
			res, 4, 8 * sck_period );
  if (verbose > 1) {
    // print out the data we sent and received
    printf( "CMD: [%02x %02x %02x %02x] [%02x %02x %02x %02x]\n",
	    cmd[0], cmd[1], cmd[2], cmd[3],
	    res[0], res[1], res[2], res[3] );
  }
  return ((nbytes == 4) &&      // should have read 4 bytes
	  res[2] == cmd[1]);              // AVR's do a delayed-echo thing
}

int main(int argc, char * argv []) {
	/*
	 * Set line buffering for file descriptors so we see stdout and stderr
	 * properly interleaved.
	 */
	setvbuf(stdout, (char*)NULL, _IOLBF, 0);
	setvbuf(stderr, (char*)NULL, _IOLBF, 0);

	progname = strrchr(argv[0],'/');

	#if defined (WIN32NATIVE)
	  /* take care of backslash as dir sep in W32 */
	  if (!progname) progname = strrchr(argv[0],'\\');
	#endif /* WIN32NATIVE */

	if (progname)
	  progname++;
	else
	  progname = argv[0];


	usbtiny_open();
	usbtiny_close();
}


