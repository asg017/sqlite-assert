COMMIT=$(shell git rev-parse HEAD)
VERSION=$(shell cat VERSION)
DATE=$(shell date +'%FT%TZ%z')

LOADABLE_CFLAGS=-fPIC -shared

ifeq ($(shell uname -s),Darwin)
CONFIG_DARWIN=y
else
CONFIG_LINUX=y
endif

ifdef CONFIG_DARWIN
LOADABLE_EXTENSION=dylib
endif

ifdef CONFIG_LINUX
LOADABLE_EXTENSION=so
endif

TARGET_LOADABLE=dist/assert0.$(LOADABLE_EXTENSION)

clean:
	rm dist/*

FORMAT_FILES=assert.h assert.c core_init.c

format: $(FORMAT_FILES)
	clang-format -i $(FORMAT_FILES)

loadable: $(TARGET_LOADABLE) $(TARGET_LOADABLE_NOFS)
cli: $(TARGET_CLI)
sqlite3: $(TARGET_SQLITE3)
sqljs: $(TARGET_SQLJS)

$(TARGET_LOADABLE): assert.c
	gcc -Isqlite \
	$(LOADABLE_CFLAGS) \
	$< -o $@

dist/sqlite3-extra.c: sqlite/sqlite3.c core_init.c
	cat sqlite/sqlite3.c core_init.c > $@

test: 
	make test-format
	make test-loadable

test-format: SHELL:=/bin/bash
test-format:
	diff -u <(cat $(FORMAT_FILES)) <(clang-format $(FORMAT_FILES))

test-loadable: $(TARGET_LOADABLE)
	python3 tests/test-loadable.py

test-watch:
	watchexec -w tests/ -w tests/ --clear make test

test-loadable-watch: $(TARGET_LOADABLE)
	watchexec -w lines.c -w $(TARGET_LOADABLE) -w tests/test-loadable.py --clear -- make test-loadable

.PHONY: all clean format \
	test test-watch test-loadable-watch \
	test-format test-loadable \
	loadable