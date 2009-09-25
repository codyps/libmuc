
#LIBUSB_PATH    = ./libusb-win32-device-bin-0.1.12.1
#LIBUSB_INCLUDE = $(LIBUSB_PATH)/include
#LIBUSB_LIB     = $(LIBUSB_PATH)/lib/gcc

#CFLAGS  = -I$(LIBUSB_INCLUDE) -g -O1
CFLAGS  = -g -O1 --std=gnu99 -Wall
#LDFLAGS = -L$(LIBUSB_LIB) -lusb -lncurses
LDFLAGS = -lusb -lncurses

#SRC = spiconsole.c usbtiny.c
SRC = spi_term.c usbtiny.c
OBJ = $(SRC:.c=.o)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

spiconsole : $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(OBJ) spiconsole spiconsole.exe

