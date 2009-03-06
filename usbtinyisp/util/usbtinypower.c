/* Search for the USBTINY */

#include <stdio.h>
#include <string.h>
#include <usb.h>
#include "usbtiny.h"
//For USBTINYISP
#define USB_VENDOR  USBTINY_VENDOR
#define USB_PRODUCT USBTINY_PRODUCT

char * progname;

usb_dev_handle *usb_device_open(void);
static int usb_control (usb_dev_handle * usb_handle,unsigned int requestid, unsigned int val, unsigned int index );

void usage(void) {
	printf("Usage: %s [off/on]\n",progname);
	exit(1);
}

int main (int argc,char **argv)
{
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
	
	

	//Actual Start

	struct usb_dev_handle *usb_handle;;
	int open_status;

	usb_init();
	//usb_set_debug(2);
	usb_handle = usb_device_open();

/*	open_status = usb_set_configuration(usb_handle,1);
	printf("conf_stat=%d\n",open_status);
	*/

	open_status = usb_claim_interface(usb_handle,0);
	if (open_status) {
		//write permisions?				
		fprintf( stderr, "%s: Error: Could not claim USB device (0x%x/0x%x; code:0x%x)\n", progname, USB_VENDOR, USB_PRODUCT, open_status );
		exit(1);
	}
	
	//printf("argc=%d\n",argc);
	//printf("argv[1]=%s\n",argv[1]);
	//printf("argv[2]=%s\n",argv[2]);
	//if (!strcmp(argv[1],"-p")) {
	if (argc==2) {
		if 	(!strcmp(argv[1],"on"))
			usb_control(usb_handle, USBTINY_POWERUP, SCK_DEFAULT, RESET_LOW);
		else if (!strcmp(argv[1],"off"))
			usb_control(usb_handle, USBTINY_POWERDOWN, 0, 0);
		else
			usage();
	}
	else
		usage();
	//}

/*	open_status = usb_set_altinterface(usb_handle,0);
	printf("alt_stat=%d\n",open_status);
	*/

/*	send_status=usb_bulk_write(xsv_handle,4,&send_data,1,500);
	printf("TX stat=%d\n",send_status);
	*/

/*	usb_bulk_read(xsv_handle,3,&receive_data,1,500);	
	printf("RX stat=%d -> RX char=%d\n",send_status,receive_data);
	*/

	/* Write the data2send bytes to the 7-segs */
  /*  	send_status = usb_control_msg
	(xsv_handle,0x20,0x09,0x0200,0x0001,send_data,2,500); 
	printf("TX stat=%d\n",send_status);
	usleep(10000);
  */ 
	/* Read the bytes that were just sent to the 7-segs */
  /*	
    	send_status = usb_control_msg(xsv_handle,0xA0,0x01,0x0100,0x0001,receive_data,2,500); 
	printf("RX stat=%d data=0x%x,0x%x\n",send_status,receive_data[0],receive_data[1]);
	usleep(10000);
  */	
	usb_close(usb_handle);
	return 1;
}	


usb_dev_handle *usb_device_open(void)
{
  struct usb_bus      *bus;
  struct usb_device   *dev = 0;
  usb_dev_handle * usb_handle;

  usb_init();                    // initialize the libusb system
  usb_find_busses();             // have libusb scan all the usb busses available
  usb_find_devices();            // have libusb scan all the usb devices available

  usb_handle = NULL;

  // now we iterate through all the busses and devices
  for ( bus = usb_busses; bus; bus = bus->next ) {
    for	( dev = bus->devices; dev; dev = dev->next ) {
      if (dev->descriptor.idVendor == USB_VENDOR
	  && dev->descriptor.idProduct == USB_PRODUCT ) {   // found match?

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
	     progname, USB_VENDOR, USB_PRODUCT );
    exit(1);
  }

  return usb_handle;                  // If we got here, we must have found a good USB device
}

// Wrapper for simple usb_control_msg messages
static int usb_control (usb_dev_handle * usb_handle,unsigned int requestid, unsigned int val, unsigned int index )
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

