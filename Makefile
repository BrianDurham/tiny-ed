CC=gcc
CFLAGS=-Wall -O2
TARGET=tiny-ed

all: $(TARGET)

$(TARGET): tiny-ed.c
	$(CC) $(CFLAGS) -o $(TARGET) tiny-ed.c

clean:
	rm -f $(TARGET)
