CC ?= ${CROSS_COMPILE}gcc
CCFLAGS ?= -g -Wall -Werror
TARGET ?= sensor_read

all: $(TARGET)

sensor_read.o: sensor_read.c
	$(CC) $(CCFLAGS) -c sensor_read.c

$(TARGET): sensor_read.o
	$(CC) sensor_read.o -o sensor_read $(LDFLAGS) $(LDARGS)

clean:
	rm -rf sensor_read.o $(TARGET)

.PHONY: all clean