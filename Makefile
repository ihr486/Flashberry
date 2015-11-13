CSRCS := $(wildcard *.c)
COBJS := $(CSRCS:%.c=%.o)

OBJS := $(COBJS)
BIN := flashberry

CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -Os
LDFLAGS :=

.PHONY: all check clean install

all: $(BIN)

check:
	$(CC) $(CFLAGS) -fsyntax-only $(CSRCS)

clean:
	-@rm -vf $(BIN) $(OBJS)

install: $(BIN)
	install --mode=755 --target-directory=/usr/local/bin $<

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
