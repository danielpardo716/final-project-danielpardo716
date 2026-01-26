CC ?= ${CROSS_COMPILE}gcc
CCFLAGS ?= -g -Wall -Werror
LDFLAGS ?= -lpthread -lrt
SRC_DIR ?= src
OBJ_DIR ?= obj
TARGET ?= aesdled

SRCS := led_blinky.c
OBJS := led_blinky.o

all: $(TARGET)

# Linking object files to create the final executable
$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^

# Compiling source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean