CC = gcc
CFLAGS = -Wall -Wextra -g -Iutils
LDFLAGS = -Lbuild -lutils

VM_TARGET = build/vm       			# Final executable
UTILS_LIB = build/libutils.so 	# Shared library
SRC_OBJS = build/main.o

.PHONY: all clean utils

all: $(VM_TARGET)

# vm depends on utils
$(VM_TARGET): $(SRC_OBJS) $(UTILS_LIB)
	$(CC) $(CFLAGS) -o $@ $^

build/main.o: src/main.c src/main.h
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

$(UTILS_LIB):
	$(MAKE) -C utils/
	mkdir -p build
	cp utils/libutils.so build/libutils.so

clean:
	$(MAKE) -C utils/ clean
	rm -rf build
