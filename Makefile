LIB_DIR = lib

.PHONY: build_lib

build_lib:
	$(MAKE) -C $(LIB_DIR)

clean:
	$(MAKE) -C $(LIB_DIR) clean