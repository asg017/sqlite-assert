VERSION=$(shell cat VERSION)

ifeq ($(shell uname -s),Darwin)
CONFIG_DARWIN=y
else ifeq ($(OS),Windows_NT)
CONFIG_WINDOWS=y
else
CONFIG_LINUX=y
endif

ifdef CONFIG_DARWIN
LOADABLE_EXTENSION=dylib
endif

ifdef CONFIG_LINUX
LOADABLE_EXTENSION=so
endif

ifdef CONFIG_WINDOWS
LOADABLE_EXTENSION=dll
endif


ifdef python
PYTHON=$(python)
else
PYTHON=python3
endif

prefix=dist
$(prefix):
	mkdir -p $(prefix)

DEFINE_ASSERT=-DSQLITE_ASSERT_VERSION="\"v$(VERSION)\""

TARGET_LOADABLE=$(prefix)/assert0.$(LOADABLE_EXTENSION)

TARGET_STATIC_ASSERT=$(prefix)/libsqlite_assert0.a
TARGET_STATIC_ASSERT_H=$(prefix)/sqlite-assert.h
TARGET_STATIC=$(TARGET_STATIC_ASSERT) $(TARGET_STATIC_ASSERT_H)


loadable: $(TARGET_LOADABLE)
static: $(TARGET_STATIC)

$(TARGET_LOADABLE): sqlite-assert.c $(prefix)
	gcc -fPIC -shared \
	-Isqlite \
	-O3 \
	$(DEFINE_ASSERT) $(CFLAGS) \
	$< -o $@

$(TARGET_STATIC_ASSERT): sqlite-assert.c $(prefix)
	gcc -Isqlite $(DEFINE_ASSERT) $(CFLAGS) -DSQLITE_CORE \
	-O3 -c  $< -o $(prefix)/assert.o
	ar rcs $@ $(prefix)/assert.o

$(TARGET_STATIC_ASSERT_H): sqlite-assert.h $(prefix)
	cp $< $@

FORMAT_FILES=sqlite-assert.h sqlite-assert.c

format: $(FORMAT_FILES)
	clang-format -i $(FORMAT_FILES)

clean:
	rm -rf dist/*

test-loadable:
	python3 tests/test-loadable.py

test:
	make test-format
	make test-loadable

test-format: SHELL:=/bin/bash
test-format:
	diff -u <(cat $(FORMAT_FILES)) <(clang-format $(FORMAT_FILES))

.PHONY: version loadable static test clean gh-release \
	ruby

gh-release:
	make version
	git add --all
	git commit -m "v$(VERSION)"
	git tag v$(VERSION)
	git push origin main v$(VERSION)
	gh release create v$(VERSION) --prerelease --notes="" --title=v$(VERSION)


TARGET_WHEELS=$(prefix)/wheels
INTERMEDIATE_PYPACKAGE_EXTENSION=bindings/python/sqlite_assert/

$(TARGET_WHEELS): $(prefix)
	mkdir -p $(TARGET_WHEELS)

bindings/ruby/lib/version.rb: bindings/ruby/lib/version.rb.tmpl VERSION
	VERSION=$(VERSION) envsubst < $< > $@

bindings/rust/Cargo.toml: bindings/rust/Cargo.toml.tmpl VERSION
	VERSION=$(VERSION) envsubst < $< > $@

bindings/rust/Cargo.lock: bindings/rust/Cargo.toml
	cargo update --manifest-path=$<

bindings/python/sqlite_assert/version.py: bindings/python/sqlite_assert/version.py.tmpl VERSION
	VERSION=$(VERSION) envsubst < $< > $@
	echo "✅ generated $@"

bindings/datasette/datasette_sqlite_assert/version.py: bindings/datasette/datasette_sqlite_assert/version.py.tmpl VERSION
	VERSION=$(VERSION) envsubst < $< > $@
	echo "✅ generated $@"

bindings/go/assert/sqlite-assert.h: sqlite-assert.h
	cp $< $@


python: $(TARGET_WHEELS) $(TARGET_LOADABLE) bindings/python/setup.py bindings/python/sqlite_assert/__init__.py scripts/rename-wheels.py
	cp $(TARGET_LOADABLE) $(INTERMEDIATE_PYPACKAGE_EXTENSION)
	rm $(TARGET_WHEELS)/*.wheel || true
	pip3 wheel bindings/python/ -w $(TARGET_WHEELS)
	python3 scripts/rename-wheels.py $(TARGET_WHEELS) $(RENAME_WHEELS_ARGS)
	echo "✅ generated python wheel"

datasette: $(TARGET_WHEELS) bindings/datasette/setup.py bindings/datasette/datasette_sqlite_assert/__init__.py
	rm $(TARGET_WHEELS)/datasette* || true
	pip3 wheel bindings/datasette/ --no-deps -w $(TARGET_WHEELS)

node: VERSION bindings/node/platform-package.README.md.tmpl bindings/node/platform-package.package.json.tmpl bindings/node/sqlite-assert/package.json.tmpl scripts/node_generate_platform_packages.sh
	scripts/node_generate_platform_packages.sh

deno: VERSION bindings/deno/deno.json.tmpl
	scripts/deno_generate_package.sh

rust: bindings/rust/Cargo.toml bindings/rust/Cargo.lock

version:
	make bindings/ruby/lib/version.rb
	make bindings/python/sqlite_assert/version.py
	make bindings/datasette/datasette_sqlite_assert/version.py
	make rust
	make node
	make deno
