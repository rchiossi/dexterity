CC = gcc
CFLAGS = -fPIC

LIB = dexterity.so

MODULES = bytestream dex dxprinter

APPS = dxread bs_test

.PHONY: all clean

all: $(LIB) $(APPS)

$(LIB): $(MODULES)
	$(CC) $(CFLAGS) -shared $(foreach mod,$(MODULES),$(mod).o) -o $@

$(MODULES): %: %.c %.h
	$(CC) $(CFLAGS) -c $@.c

$(APPS): %: %.c $(foreach mod,$(MODULES),$(mod).o)
	$(CC) $(CFLAGS) -o $@ $(foreach mod,$(MODULES),$(mod).o) $@.c
#	$(CC) $(CFLAGS) -o $@ $@.c $(LIB)

clean:
	rm -f $(foreach mod,$(MODULES),$(mod).o) $(APPS) $(LIB)
