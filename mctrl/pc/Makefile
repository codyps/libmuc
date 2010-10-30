CC = gcc
RM = rm -f

TARGET = hdlc_frame

SRC = main.c

CFLAGS = -Wall

all: build

rebuild: | clean build

build: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	$(RM) $(TARGET)
