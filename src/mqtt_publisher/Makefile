CC ?= ${CROSS_COMPILE}gcc
CCFLAGS ?= -g -Wall -Werror
LDARGS := -lmosquitto -lpthread -lrt
TARGET ?= mqtt_publisher

all: $(TARGET)

mqtt_publisher.o: mqtt_publisher.c
	$(CC) $(CCFLAGS) -c mqtt_publisher.c

$(TARGET): mqtt_publisher.o
	$(CC) mqtt_publisher.o -o mqtt_publisher $(LDFLAGS) $(LDARGS)

clean:
	rm -rf mqtt_publisher.o $(TARGET)

.PHONY: all clean