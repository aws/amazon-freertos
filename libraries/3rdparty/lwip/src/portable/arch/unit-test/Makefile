TEST_SRC:=$(wildcard test-*.c)
TEMP_SRC:=$(patsubst test-%,%,$(TEST_SRC))
TESTS=$(patsubst %.c,%,$(TEST_SRC))

CFLAGS+=-std=c99 -Wall -fprofile-arcs -ftest-coverage -O0 -ggdb

build: $(TESTS)

test: build
	@rm -f *.gcov *.gcda
	@./report $(TESTS)

clean:
	rm -f $(TESTS) $(TEMP_SRC) *.gc??

.PHONY: build test clean

$(TEMP_SRC): ../sys_arch.c
	sed -nr '/^(void|u32_t|err_t) $(patsubst %.c,%,$@)\(.*[^;]$$/,/^}/p' $< > $@

$(TESTS): test-%: test-%.c %.c common.h tap.h
	$(CC) $(CFLAGS) $< -o $@
