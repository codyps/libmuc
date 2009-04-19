#include "usbtiny.h"
#include <usb.h>
#include 

usb_dev_handle *usb_handle;
int sck_period;
int chunk_size;

char * progname;


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


