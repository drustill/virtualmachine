CC = gcc
# -03 = Optimization; -fPIC = Position-Independent (load at any memory address)
CFLAGS = -O3 -Wall -std=c2x -fPIC
LDFLAGS = -shared -Wl,-install_name,@rpath/libutils.so -Wl,-rpath,@loader_path

TARGET = ../build/libutils.so
SRCS = utils.c
OBJS = ../build/utils.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

../build/utils.o: utils.c
	mkdir -p ../build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ../build/utils.o ../build/libutils.so
