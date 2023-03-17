EXECUTABLE := leet_test

CC := gcc

CFLAGS := -Wall -g
# CFLAGS += -Werror

SRCS := $(wildcard *.c)

# OBJS := $(SRCS:.c=.o)
OBJS := $(patsubst %.c, %.o, $(SRCS))

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE)