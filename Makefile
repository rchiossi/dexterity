CC = gcc
CFLAGS =

MODULES = bytestream dex

APPS = tester

.PHONY: all clean

all: $(MODULES) $(APPS)

modules: $(MODULES)

$(MODULES): %: %.c %.h
	$(CC) $(CFLAGS) -c $@.c

tester: %: %.c $(foreach mod,$(MODULES),$(mod).o)
	$(CC) $(CFLAGS) -o $@ $(foreach mod,$(MODULES),$(mod).o) $@.c

clean:
	rm -f *.o $(APPS)
