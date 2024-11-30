CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = build/libutils.so

.PHONY: all clean utils

# Default target builds the utils library
all: $(TARGET)

# Build the utils library
$(TARGET):
	$(MAKE) -C utils/

# Clean both the utils directory and the build directory
clean:
	$(MAKE) -C utils/ clean
	rm -rf build
