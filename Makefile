CC = gcc
CFLAGS = -fPIC

LIB = dexterity.so

CORE = bytestream dex dex_builder dex_parser

MODULES = dxprinter

APPS = dxread bs_test builder

.PHONY: all clean

all: $(LIB) $(APPS)

$(LIB): $(CORE) $(MODULES)
	$(CC) $(CFLAGS) -shared $(foreach mod,$(MODULES),$(mod).o) -o $@

$(CORE): %: %.c dex.h
	$(CC) $(CFLAGS) -c $@.c

$(MODULES): %: %.c %.h
	$(CC) $(CFLAGS) -c $@.c

$(APPS): %: %.c $(foreach core,$(CORE),$(core).o) $(foreach mod,$(MODULES),$(mod).o)
	$(CC) $(CFLAGS) -o $@ $(foreach core,$(CORE),$(core).o) $(foreach mod,$(MODULES),$(mod).o) $@.c
#	$(CC) $(CFLAGS) -o $@ $@.c $(LIB)

clean:
	rm -f $(LIB) $(foreach core,$(CORE),$(core).o) $(foreach mod,$(MODULES),$(mod).o) $(APPS)
