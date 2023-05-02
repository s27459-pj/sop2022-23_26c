CC=gcc
CFLAGS=-Wall --pedantic --ansi
# Find all .c files in the current directory
TARGETS=$(shell find . -maxdepth 1 -type f -name "*.c" -printf "%f\n" | sed 's/\.c//g')

.PHONY: clean .FORCE

$(TARGETS): .FORCE
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -fv $(TARGETS)
