/*
  This file is appended to the end of a sqlite3.c amalgammation
  file to include sqlite-assert functions/tables statically in
  a build.
*/
#include "sqlite-assert.h"
int core_init(const char *dummy) {
  return sqlite3_auto_extension((void *)sqlite3_assert_init);
}