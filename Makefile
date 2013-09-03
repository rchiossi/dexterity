CC = gcc
CFLAGS = -fPIC -std=gnu99 -g

LIB = dexterity.so

CORE = bytestream dex_builder dex_parser leb128

.PHONY: all clean

all: $(LIB) $(APPS)

$(LIB): $(CORE) $(MODULES)
	$(CC) $(CFLAGS) -shared $(foreach mod,$(CORE),$(mod).o) -o $@

$(CORE): %: %.c dex.h
	$(CC) $(CFLAGS) -c $@.c

clean:
	rm -f $(LIB) $(foreach core,$(CORE),$(core).o) $(foreach mod,$(MODULES),$(mod).o) $(APPS)
