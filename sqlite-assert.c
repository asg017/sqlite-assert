#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1

#define REASON_TYPE_MISMATCH 1
#define REASON_TEXT_LENGTH_MISMATCH 2
#define REASON_BLOB_LENGTH_MISMATCH 3
#define REASON_VALUE_MISMATCH 4

int values_equal(sqlite3_value *lValue, sqlite3_value *rValue, int *reason) {
  int ltype = sqlite3_value_type(lValue);
  int rtype = sqlite3_value_type(rValue);

  if (ltype != rtype) {
    *reason = REASON_TYPE_MISMATCH;
    return 0;
  }

  switch (ltype) {
  case SQLITE_INTEGER: {
    int l = sqlite3_value_int64(lValue);
    int r = sqlite3_value_int64(rValue);
    if (l == r)
      return 1;

    *reason = REASON_VALUE_MISMATCH;
    return 0;
  }
  case SQLITE_FLOAT: {
    double l = sqlite3_value_double(lValue);
    double r = sqlite3_value_double(rValue);
    if (l == r)
      return 1;

    *reason = REASON_VALUE_MISMATCH;
    return 0;
  }
  case SQLITE_TEXT:
  case SQLITE_BLOB: {
    int lSize = sqlite3_value_bytes(lValue);
    int rSize = sqlite3_value_bytes(rValue);

    if (lSize != rSize) {
      *reason = ltype == SQLITE_TEXT ? REASON_TEXT_LENGTH_MISMATCH
                                     : REASON_BLOB_LENGTH_MISMATCH;
      return 0;
    }
    const void *l = (const void *)sqlite3_value_blob(lValue);
    const void *r = (const void *)sqlite3_value_blob(rValue);
    int rc = memcmp(l, r, lSize);
    if (rc == 0)
      return 1;

    *reason = REASON_VALUE_MISMATCH;
    return 0;
  }
  case SQLITE_NULL: {
    return 1;
  }
  }
  return 0;
}

static const char *azType[] = {"integer", "real", "text", "blob", "null"};

static const char *typeName(sqlite3_value *v) {
  return azType[sqlite3_value_type(v) - 1];
}

/** assert(result [, message])
 *
 *
 */
static void assertFunc(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  int type = sqlite3_value_type(argv[0]);
if ((type == SQLITE_INTEGER || type == SQLITE_FLOAT) && sqlite3_value_int(argv[0]) == 0) {
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
    return;
  }
  sqlite3_result_int(ctx, 1);
}

/** assert_equal(value1, value2 [, message])
 *
 *
 */
static void assertEqualFunc(sqlite3_context *ctx, int argc,
                            sqlite3_value **argv) {
  int reason;
  if (values_equal(argv[0], argv[1], &reason)) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  switch (reason) {
  case REASON_TYPE_MISMATCH: {
    char *zMsg;
    if (argc > 2)
      zMsg = sqlite3_mprintf("Assertion error: Type mismatch, %s != %s - %s",
                             typeName(argv[0]), typeName(argv[1]),
                             sqlite3_value_text(argv[2]));
    else
      zMsg = sqlite3_mprintf("Assertion error: Type mismatch, %s != %s",
                             typeName(argv[0]), typeName(argv[1]));

    sqlite3_result_error(ctx, zMsg, -1);
    sqlite3_free(zMsg);
    return;
  }
  case REASON_VALUE_MISMATCH: {
    // TODO use sqlite3_string builder here - depending on the type of argv[0],
    // properly format l/r values (%d vs %f etc), and optionally add argv[2]
    // suffix if exists
    char *zMsg;

    sqlite3_str *str = sqlite3_str_new(NULL);
    sqlite3_str_appendall(str, "Assertion error: Value mismatch ");

    if (argc > 2) {
      sqlite3_str_appendf(str, "(%s) ",
                          (const char *)sqlite3_value_text(argv[2]));
    }

    if (sqlite3_value_type(argv[0]) == SQLITE_INTEGER) {
      sqlite3_str_appendf(str, "%d", sqlite3_value_int(argv[0]));
      sqlite3_str_appendall(str, " != ");
      sqlite3_str_appendf(str, "%d", sqlite3_value_int(argv[1]));
    } else if (sqlite3_value_type(argv[0]) == SQLITE_FLOAT) {
      sqlite3_str_appendf(str, "%f", sqlite3_value_double(argv[0]));
      sqlite3_str_appendall(str, " != ");
      sqlite3_str_appendf(str, "%f", sqlite3_value_double(argv[1]));
    }
    // TEXT or BLOB, since NULLs are always equal
    else {
      sqlite3_str_appendf(str, "\"%s\"", sqlite3_value_text(argv[0]));
      sqlite3_str_appendall(str, " != ");
      sqlite3_str_appendf(str, "\"%s\"", sqlite3_value_text(argv[1]));
    }
    zMsg = sqlite3_str_finish(str);
    if (zMsg == NULL) {
      sqlite3_result_error_nomem(ctx);
    } else {
      sqlite3_result_error(ctx, zMsg, -1);
      sqlite3_free(zMsg);
    }
    return;
  }
  case REASON_TEXT_LENGTH_MISMATCH: {
    int lSize = sqlite3_value_bytes(argv[0]);
    int rSize = sqlite3_value_bytes(argv[1]);
    char *zMsg = sqlite3_mprintf(
        "Assertion error: Text length mismatch, %d != %d", lSize, rSize);
    sqlite3_result_error(ctx, zMsg, -1);
    sqlite3_free(zMsg);
    return;
  }
  case REASON_BLOB_LENGTH_MISMATCH: {
    int lSize = sqlite3_value_bytes(argv[0]);
    int rSize = sqlite3_value_bytes(argv[1]);
    char *zMsg = sqlite3_mprintf(
        "Assertion error: Blob length mismatch, %d != %d", lSize, rSize);
    sqlite3_result_error(ctx, zMsg, -1);
    sqlite3_free(zMsg);
    return;
  }
  }
}

/** assert_type(value, type [, message])
 *
 *
 */
static void assertTypeFunc(sqlite3_context *ctx, int argc,
                           sqlite3_value **argv) {

  int actual = sqlite3_value_type(argv[0]);
  char *expected = (char *)sqlite3_value_text(argv[1]);

  if (actual == SQLITE_TEXT && sqlite3_stricmp(expected, "text") == 0) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  if (actual == SQLITE_INTEGER && (sqlite3_stricmp(expected, "integer") == 0 ||
                                   sqlite3_stricmp(expected, "int") == 0)) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  if (actual == SQLITE_FLOAT && (sqlite3_stricmp(expected, "float") == 0 ||
                                 sqlite3_stricmp(expected, "real") == 0)) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  if (actual == SQLITE_BLOB && sqlite3_stricmp(expected, "blob") == 0) {
    sqlite3_result_int(ctx, 1);
    return;
  }
  char *zMsg;
  if (argc > 2)
    zMsg = sqlite3_mprintf("Assertion error: %s", sqlite3_value_text(argv[1]));
  else {
    zMsg = sqlite3_mprintf(
        "Assertion error: Type mismatch, expected '%s' but got '%s'", expected,
        typeName(argv[0]));
  }

  sqlite3_result_error(ctx, zMsg, -1);
  sqlite3_free(zMsg);
}

/** assert_subtype(value, subtype [, message])
 *
 *
 */
static void assertSubtypeFunc(sqlite3_context *ctx, int argc,
                              sqlite3_value **argv) {
  unsigned int actualSubtype = sqlite3_value_subtype(argv[0]);
  int expectedType = sqlite3_value_type(argv[1]);
  /*if (expectedType == SQLITE_TEXT) {
    if (sqlite3_stricmp(sqlite3_value_text(argv[1]), "json") == 0) {
    } else {
      sqlite3
    }
  } else */
  if (expectedType == SQLITE_INTEGER) {
    int expected = sqlite3_value_int(argv[1]);
    if (actualSubtype == expected) {
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

  sqlite3_create_function(db, "assert_type", 2, flags, 0, assertTypeFunc, 0, 0);
  sqlite3_create_function(db, "assert_type", 3, flags, 0, assertTypeFunc, 0, 0);

  sqlite3_create_function(db, "assert_subtype", 2, flags, 0, assertSubtypeFunc,
                          0, 0);
  sqlite3_create_function(db, "assert_subtype", 3, flags, 0, assertSubtypeFunc,
                          0, 0);
  return SQLITE_OK;
}
