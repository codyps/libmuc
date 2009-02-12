
LIBUSB_PATH    = ./libusb-win32-device-bin-0.1.12.1
LIBUSB_INCLUDE = $(LIBUSB_PATH)/include
LIBUSB_LIB     = $(LIBUSB_PATH)/lib/gcc

CFLAGS  = -I$(LIBUSB_INCLUDE) -g -O1
LDFLAGS = -L$(LIBUSB_LIB) -lusb -lncurses

SRC = spiconsole.c
OBJ = $(SRC:.c=.o)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

spiconsole : $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(OBJ) spiconsole spiconsole.exe

