CC = gcc
CFLAGS = -Wall -Werror -pedantic -O3

.PHONY: all
all: vm

vm: src/vm.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f vm
