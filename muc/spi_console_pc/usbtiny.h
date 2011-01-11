
#ifndef _USBTINY_H_
#define _USBTINY_H_

int usbtiny_spi(unsigned char cmd[4], unsigned char res[4]);

void usbtiny_set_chunk_size(int period);
int usbtiny_set_sck_period(double v);

int usbtiny_open(void);
void usbtiny_close(void);

int usb_control(unsigned int requestid, unsigned int val, unsigned int index);
#endif
