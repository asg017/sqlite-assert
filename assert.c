/*
** This SQLite extension implements xxx
*/
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <string.h>
#include <stdio.h>


/*
** Implementation of the assert() function.
** TODO docs
*/
static void assertfunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  sqlite3 *db = sqlite3_context_db_handle(context);
  
  assert( argc==1 || argc==2);
  int i = sqlite3_value_int(argv[0]);
  if(i == 0) {
    char *zMsg;
    if(argc ==2) {
      const char *iMsg = (const char*)sqlite3_value_text(argv[1]);
      char *zMsg = sqlite3_mprintf("Assertion error: %s", iMsg);
      sqlite3_result_error(context, zMsg, -1);
      sqlite3_result_error(context, zMsg, -1);
      sqlite3_free(zMsg);
    }
    else {
      zMsg = "Assert error.";
      sqlite3_result_error(context, zMsg, -1);
    }
  }
  else {
    //sqlite3_result_int(context, i);
    sqlite3_result_text(context, "✅", 1, SQLITE_TRANSIENT);
  }
}

/*
** Implementation of the assert_query() function.
** TODO docs
*/
static void assertqueryfunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  sqlite3 *db = sqlite3_context_db_handle(context);
  assert( argc==1 || argc==2);
  
  const char *zSql = (const char*)sqlite3_value_text(argv[0]);
  sqlite3_stmt *pStmt = 0;
  int nCol;                   /* Number of columns in the result set */
  int i;                      /* Loop counter */
  int rc;
  int n;
  const char *z;
  
  if( zSql==0 ) return;
  
   while( zSql[0] ){
    rc = sqlite3_prepare_v2(db, zSql, -1, &pStmt, &zSql);
    if( rc ){
      char *zMsg = sqlite3_mprintf("error SQL statement [%s]: %s",
                                   zSql, sqlite3_errmsg(db));
      sqlite3_finalize(pStmt);
      sqlite3_result_error(context, zMsg, -1);
      sqlite3_free(zMsg);
      return;
    }
    if( !sqlite3_stmt_readonly(pStmt) ){
      char *zMsg = sqlite3_mprintf("non-query: [%s]", sqlite3_sql(pStmt));
      sqlite3_finalize(pStmt);
      sqlite3_result_error(context, zMsg, -1);
      sqlite3_free(zMsg);
      return;
    }
    nCol = sqlite3_column_count(pStmt);
    z = sqlite3_sql(pStmt);
    n = (int)strlen(z);
    
    printf("starting \n");
    int result;
    while(1){
      result = sqlite3_step(pStmt);
      printf("result=%d\n", result);
      switch(result) {
        case SQLITE_DONE:
        case SQLITE_INTERRUPT:
          goto success;
        case SQLITE_ERROR:
          sqlite3_finalize(pStmt);
          if(argc ==2) {
            const char *iMsg = (const char*)sqlite3_value_text(argv[1]);
            char *zMsg = sqlite3_mprintf("Assertion error: %s", iMsg);
            sqlite3_result_error(context, zMsg, -1);
            sqlite3_result_error(context, zMsg, -1);
            sqlite3_free(zMsg);
          }
          else {
            char *zMsg = "Assert error.";
            sqlite3_result_error(context, zMsg, -1);
          }
          for(i=0; i<nCol; i++){
            switch( sqlite3_column_type(pStmt,i) ){
              case SQLITE_NULL: {
                break;
              }
              case SQLITE_INTEGER: {
                sqlite3_int64 v = sqlite3_column_int64(pStmt,i);
                break;
              }
              case SQLITE_FLOAT: {
                double r = sqlite3_column_double(pStmt,i);
                break;
              }
              case SQLITE_TEXT: {
                const unsigned char *z2 = sqlite3_column_text(pStmt, i);
                break;
              }
              case SQLITE_BLOB: {
                const unsigned char *z2 = sqlite3_column_blob(pStmt, i);
                break;
              }
            }
          }
          return;

      }
      if(result == SQLITE_DONE || result == SQLITE_INTERRUPT) break;
      
      //for(i=0; i<nCol; i++){}
    }
    success:
    printf("done?\n");
    sqlite3_finalize(pStmt);
  }
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_assert_init(
  sqlite3 *db, 
  char **pzErrMsg, 
  const sqlite3_api_routines *pApi
){
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg;  /* Unused parameter */
  rc = sqlite3_create_function(db, "assert", 1,
                   SQLITE_UTF8|SQLITE_INNOCUOUS|SQLITE_DETERMINISTIC,
                   0, assertfunc, 0, 0);
  rc = sqlite3_create_function(db, "assert", 2,
                   SQLITE_UTF8|SQLITE_INNOCUOUS|SQLITE_DETERMINISTIC,
                   0, assertfunc, 0, 0);
  rc = sqlite3_create_function(db, "assert_query", 1,
                   SQLITE_UTF8|SQLITE_INNOCUOUS|SQLITE_DETERMINISTIC,
                   0, assertqueryfunc, 0, 0);
  rc = sqlite3_create_function(db, "assert_query", 2,
                   SQLITE_UTF8|SQLITE_INNOCUOUS|SQLITE_DETERMINISTIC,
                   0, assertqueryfunc, 0, 0);
  return rc;
}
