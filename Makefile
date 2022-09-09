CFLAGS := -std=gnu99 -Wall -Werror -Wno-unused-function -Wfatal-errors -Os

.PHONY: all
all: muasm

.PHONY: test
test: muasm-test

%: %.c
	gcc $(CFLAGS) "$<" -o "$@"

%-test: %.c
	gcc $(CFLAGS) -D DEBUG "$<" -o "$@"

.PHONY: clean
clean:
	@rm -fv muasm muasm.o muasm-test muasm-test.o
