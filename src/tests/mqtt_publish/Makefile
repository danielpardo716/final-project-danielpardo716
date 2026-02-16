CC ?= ${CROSS_COMPILE}gcc
CCFLAGS ?= -g -Wall -Werror
LDARGS := -lmosquitto -lpthread -lrt
TARGET ?= mqtt_publish

all: $(TARGET)

mqtt_publish.o: mqtt_publish.c
	$(CC) $(CCFLAGS) -c mqtt_publish.c

$(TARGET): mqtt_publish.o
	$(CC) mqtt_publish.o -o mqtt_publish $(LDFLAGS) $(LDARGS)

clean:
	rm -rf mqtt_publish.o $(TARGET)

.PHONY: all clean