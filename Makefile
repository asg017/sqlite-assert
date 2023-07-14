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

TARGET_OBJ=dist/libsqliteassert.o
TARGET_AR=dist/libsqliteassert.a
TARGET_LOADABLE=dist/assert0.$(LOADABLE_EXTENSION)
TARGET_SQLITE3=dist/sqlite3

clean:
	rm dist/*

FORMAT_FILES=sqlite-assert.h sqlite-assert.c core_init.c

format: $(FORMAT_FILES)
	clang-format -i $(FORMAT_FILES)

loadable: $(TARGET_LOADABLE) $(TARGET_LOADABLE_NOFS)
sqlite3: $(TARGET_SQLITE3)

$(TARGET_LOADABLE): sqlite-assert.c
	gcc -Isqlite \
	$(LOADABLE_CFLAGS) \
	$< -o $@

$(TARGET_OBJ): sqlite-assert.c 
	gcc -Isqlite \
	-c \
	-DSQLITE_CORE \
	$< -o $@

$(TARGET_AR): $(TARGET_OBJ)
	ar rcs $@ $<

$(TARGET_SQLITE3): dist/sqlite3-extra.c sqlite/shell.c sqlite-assert.c
	gcc \
	-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION=1 \
	-DSQLITE_EXTRA_INIT=core_init \
	-I./ -I./sqlite dist/sqlite3-extra.c sqlite/shell.c sqlite-assert.c -o $@

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
