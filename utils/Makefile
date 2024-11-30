CC = gcc
# -03 = Optimization; -fPIC = Position-Independent (load at any memory address)
CFLAGS = -O3 -Wall -std=c2x -fPIC
LDFLAGS = -shared

TARGET = libutils.so
SRCS = utils.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
