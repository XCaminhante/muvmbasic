CFLAGS := -std=gnu99 -Wall -Werror -Wno-unused-function -Wfatal-errors -Os

.PHONY: all
all: muasm

muasm: muasm.c
	gcc $(CFLAGS) $< -o $@
