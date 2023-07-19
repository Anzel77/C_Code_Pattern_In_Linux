EXECUTABLE := http_client

CC := gcc

CFLAGS := -Wall -g
# CFLAGS += -Werror
CFLAGS += -I../

# LDLIBS=-lm
# LDFLAGS=-L/usr/local/include

SRCS := $(wildcard *.c)

# OBJS := $(SRCS:.c=.o)
OBJS := $(patsubst %.c, %.o, $(SRCS))

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXECUTABLE)