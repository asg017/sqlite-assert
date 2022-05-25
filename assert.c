#include <assert.h>
#include <string.h>

#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1

static void assertFunc(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int result = sqlite3_value_int(argv[0]);
  if (result == 1) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  char *zMsg = 0;
  if (argc > 1)
    zMsg = sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[1]));
  else
    zMsg = sqlite3_mprintf("Assertion error");
  if (zMsg == 0) {
    sqlite3_result_error_nomem(ctx);
    return;
  }
  sqlite3_result_error(ctx, zMsg, -1);
  sqlite3_free(zMsg);
}

static const char *azType[] = {"integer", "real", "text", "blob", "null"};
static const char *typeName(sqlite3_value *v) {
  return azType[sqlite3_value_type(v) - 1];
}

static void assertEqualFunc(sqlite3_context *ctx, int argc,
                            sqlite3_value **argv) {
  int ltype = sqlite3_value_type(argv[0]);
  int rtype = sqlite3_value_type(argv[1]);

  if (ltype != rtype) {
    char *zMsg;
    if (argc > 2)
      zMsg =
          sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[2]));
    else
      zMsg = sqlite3_mprintf("Assertion error: Type mismatch, %s != %s",
                             typeName(argv[0]), typeName(argv[1]));

    sqlite3_result_error(ctx, zMsg, -1);
    sqlite3_free(zMsg);
    return;
  }

  switch (ltype) {
  case SQLITE_INTEGER: {
    int l = sqlite3_value_int64(argv[0]);
    int r = sqlite3_value_int64(argv[1]);
    if (l == r) {
      sqlite3_result_int(ctx, 1);
      return;
    }
    char *zMsg;
    if (argc > 2)
      zMsg =
          sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[2]));
    else
      zMsg = sqlite3_mprintf("Assertion error: %d != %d", l, r);

    sqlite3_result_error(ctx, zMsg, -1);
    sqlite3_free(zMsg);
    return;
  }
  case SQLITE_FLOAT: {
    double l = sqlite3_value_double(argv[0]);
    double r = sqlite3_value_double(argv[1]);
    if (l == r) {
      sqlite3_result_int(ctx, 1);
      return;
    }
    char *zMsg;
    if (argc > 2)
      zMsg =
          sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[2]));
    else
      zMsg = sqlite3_mprintf("Assertion error: %f != %f", l, r);

    sqlite3_result_error(ctx, zMsg, -1);
    sqlite3_free(zMsg);
    return;

    return;
  }
  case SQLITE_TEXT:
  case SQLITE_BLOB: {
    int lSize = sqlite3_value_bytes(argv[0]);
    int rSize = sqlite3_value_bytes(argv[0]);

    // TODO early catch, check sizes?

    const void *l = (const void *)sqlite3_value_blob(argv[0]);
    const void *r = (const void *)sqlite3_value_blob(argv[1]);
    int rc = memcmp(l, r, lSize);
    if (rc == 0) {
      sqlite3_result_int(ctx, 1);
      return;
    }
    if (l == r) {
      sqlite3_result_int(ctx, 1);
      return;
    }
    char *zMsg;
    if (argc > 2)
      zMsg =
          sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[2]));
    else
      // TODO: shorten long stings like
      // https://github.com/python/cpython/blob/3.10/Lib/unittest/util.py#L8-L22
      // and maybe don't do BLOBs?
      zMsg = sqlite3_mprintf("Assertion error: \"%s\" != \"%s\"", l, r);

    sqlite3_result_error(ctx, zMsg, -1);
    sqlite3_free(zMsg);
    return;
  }
  case SQLITE_NULL: {
    sqlite3_result_int(ctx, 1);
    return;
  }
  }

  char *zMsg;
  if (argc > 1)
    zMsg = sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[1]));
  else
    zMsg = sqlite3_mprintf("Assertion error");

  sqlite3_result_error(ctx, zMsg, -1);
  sqlite3_free(zMsg);
}

static void assertNotequalFunc(sqlite3_context *ctx, int argc,
                               sqlite3_value **argv) {
  int result = sqlite3_value_int(argv[0]);
  if (result == 1) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  char *zMsg;
  if (argc > 1)
    zMsg = sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[1]));
  else
    zMsg = sqlite3_mprintf("Assertion error");

  sqlite3_result_error(ctx, zMsg, -1);
  sqlite3_free(zMsg);
}

static void assertNullFunc(sqlite3_context *ctx, int argc,
                           sqlite3_value **argv) {
  int result = sqlite3_value_type(argv[0]);
  if (result == SQLITE_NULL) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  char *zMsg;
  if (argc > 1)
    zMsg = sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[1]));
  else
    zMsg = sqlite3_mprintf("Assertion error, value not not null");

  sqlite3_result_error(ctx, zMsg, -1);
  sqlite3_free(zMsg);
}

static void assertNotnullFunc(sqlite3_context *ctx, int argc,
                              sqlite3_value **argv) {
  int result = sqlite3_value_int(argv[0]);
  if (result == 1) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  char *zMsg;
  if (argc > 1)
    zMsg = sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[1]));
  else
    zMsg = sqlite3_mprintf("Assertion error");

  sqlite3_result_error(ctx, zMsg, -1);
  sqlite3_free(zMsg);
}

static void assertTypeFunc(sqlite3_context *ctx, int argc,
                           sqlite3_value **argv) {
  int actual = sqlite3_value_type(argv[0]);
  char *expected = (char *)sqlite3_value_text(argv[1]);

  /*switch(expected) {
    case "null": {

    }
  }*/
  if (actual == 1) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  char *zMsg;
  if (argc > 1)
    zMsg = sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[1]));
  else
    zMsg = sqlite3_mprintf("Assertion error");

  sqlite3_result_error(ctx, zMsg, -1);
  sqlite3_free(zMsg);
}

static void assertSubtypeFunc(sqlite3_context *ctx, int argc,
                              sqlite3_value **argv) {
  int result = sqlite3_value_int(argv[0]);
  if (result == 1) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  char *zMsg;
  if (argc > 1)
    zMsg = sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[1]));
  else
    zMsg = sqlite3_mprintf("Assertion error");

  sqlite3_result_error(ctx, zMsg, -1);
  sqlite3_free(zMsg);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
    int sqlite3_assert_init(sqlite3 *db, char **pzErrMsg,
                            const sqlite3_api_routines *pApi) {
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg; /* Unused parameter */
  const int flags = SQLITE_UTF8 | SQLITE_INNOCUOUS | SQLITE_DETERMINISTIC;
  sqlite3_create_function(db, "assert", 1, flags, 0, assertFunc, 0, 0);
  sqlite3_create_function(db, "assert", 2, flags, 0, assertFunc, 0, 0);

  sqlite3_create_function(db, "assert_equal", 2, flags, 0, assertEqualFunc, 0,
                          0);
  sqlite3_create_function(db, "assert_equal", 3, flags, 0, assertEqualFunc, 0,
                          0);

  sqlite3_create_function(db, "assert_notequal", 2, flags, 0,
                          assertNotequalFunc, 0, 0);
  sqlite3_create_function(db, "assert_notequal", 3, flags, 0,
                          assertNotequalFunc, 0, 0);

  sqlite3_create_function(db, "assert_null", 1, flags, 0, assertNullFunc, 0, 0);
  sqlite3_create_function(db, "assert_null", 2, flags, 0, assertNullFunc, 0, 0);

  sqlite3_create_function(db, "assert_notnull", 1, flags, 0, assertNotnullFunc,
                          0, 0);
  sqlite3_create_function(db, "assert_notnull", 2, flags, 0, assertNotnullFunc,
                          0, 0);

  sqlite3_create_function(db, "assert_type", 2, flags, 0, assertTypeFunc, 0, 0);
  sqlite3_create_function(db, "assert_type", 3, flags, 0, assertTypeFunc, 0, 0);

  sqlite3_create_function(db, "assert_subtype", 2, flags, 0, assertSubtypeFunc,
                          0, 0);
  sqlite3_create_function(db, "assert_subtype", 3, flags, 0, assertSubtypeFunc,
                          0, 0);
  return SQLITE_OK;
}
