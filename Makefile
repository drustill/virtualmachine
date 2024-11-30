CC = gcc
CFLAGS = -Wall -Wextra -g -Iutils
LDFLAGS = -Lbuild -lutils -Wl,-rpath,@loader_path

VM_TARGET = build/vm
UTILS_LIB = build/libutils.so
SRC_OBJS = build/main.o

.PHONY: all clean utils

# Final executable
all: $(VM_TARGET)

# Depends on utils
$(VM_TARGET): $(SRC_OBJS) $(UTILS_LIB)
	$(CC) $(CFLAGS) -o $@ $(SRC_OBJS) $(LDFLAGS)
	# $(CC) $(CFLAGS) -o $@ $(SRC_OBJS) -Lbuild -lutils -Wl,-rpath,@loader_path

build/main.o: src/main.c src/main.h
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

$(UTILS_LIB):
	$(MAKE) -C utils/

clean:
	$(MAKE) -C utils/ clean
	rm -rf build
