CC = gcc
CFLAGS = -fPIC -std=gnu99 -W -Wall -Wextra -pedantic

LIB = libdexterity.so

CORE = bytestream dex_builder dex_parser dex_string dex_offset dex_stringid leb128 malloc_s

.PHONY: all clean

all: $(LIB) $(APPS)

$(LIB): $(CORE) $(MODULES)
	$(CC) $(CFLAGS) -shared $(foreach mod,$(CORE),$(mod).o) -o $@

$(CORE): %: %.c dex.h
	$(CC) $(CFLAGS) -c $@.c

clean:
	rm -f $(LIB) $(foreach core,$(CORE),$(core).o)
