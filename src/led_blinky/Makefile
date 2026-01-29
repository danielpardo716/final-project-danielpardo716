CC ?= ${CROSS_COMPILE}gcc
CCFLAGS ?= -g -Wall -Werror
LDARGS := -lgpiod -lpthread -lrt
TARGET ?= led_blinky

all: $(TARGET)

led_blinky.o: led_blinky.c
	$(CC) $(CCFLAGS) -c led_blinky.c

$(TARGET): led_blinky.o
	$(CC) led_blinky.o -o led_blinky $(LDFLAGS) $(LDARGS)

clean:
	rm -rf led_blinky.o $(TARGET)

.PHONY: all clean