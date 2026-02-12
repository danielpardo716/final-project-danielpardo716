CC ?= ${CROSS_COMPILE}gcc
CCFLAGS ?= -g -Wall -Werror
PUB_LDARGS := -lmosquitto -lpthread -lrt
SUB_LDARGS := -lgpiod -lmosquitto -lpthread -lrt

all: mqtt_publisher mqtt_subscriber

mqtt_publisher.o: mqtt_publisher.c
	$(CC) $(CCFLAGS) -c mqtt_publisher.c

mqtt_publisher: mqtt_publisher.o
	$(CC) mqtt_publisher.o -o mqtt_publisher $(LDFLAGS) $(PUB_LDARGS)

mqtt_subscriber.o: mqtt_subscriber.c
	$(CC) $(CCFLAGS) -c mqtt_subscriber.c

mqtt_subscriber: mqtt_subscriber.o
	$(CC) mqtt_subscriber.o -o mqtt_subscriber $(LDFLAGS) $(SUB_LDARGS)

clean:
	rm -rf mqtt_publisher.o mqtt_subscriber.o mqtt_publisher mqtt_subscriber

.PHONY: all mqtt_publisher mqtt_subscriber clean