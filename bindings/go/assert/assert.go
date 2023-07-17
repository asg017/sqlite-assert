// yoyo
package assert

// #cgo LDFLAGS: -lsqlite_assert0
// #cgo CFLAGS: -DSQLITE_CORE
// #include <sqlite3ext.h>
// #include "sqlite-assert.h"
//
import "C"

// Once called, every future new SQLite3 connection created in this process
// will have the assert extension loaded. It will persist until [Cancel] is
// called.
//
// Calls [sqlite3_auto_extension()] under the hood.
//
// [sqlite3_auto_extension()]: https://www.sqlite.org/c3ref/auto_extension.html
func Auto() {
	C.sqlite3_auto_extension( (*[0]byte) ((C.sqlite3_assert_init)) );
}

// "Cancels" any previous calls to [Auto]. Any new SQLite3 connections created
// will not have the assert extension loaded.
//
// Calls sqlite3_cancel_auto_extension() under the hood.
func Cancel() {
	C.sqlite3_cancel_auto_extension( (*[0]byte) (C.sqlite3_assert_init) );
}
