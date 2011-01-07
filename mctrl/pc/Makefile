CC = gcc
RM = rm -f

TARGET = hdlc_frame

SRC = main.c frame_async.c

CFLAGS = -ggdb
override CFLAGS += -Wall -pipe

all: build

rebuild: | clean build

build: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	$(RM) $(TARGET)
