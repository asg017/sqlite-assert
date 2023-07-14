#ifndef _SQLITE_ASSERT_H
#define _SQLITE_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

int sqlite3_assert_init(sqlite3 *db, char **pzErrMsg,
                        const sqlite3_api_routines *pApi);

#ifdef __cplusplus
} /* end of the 'extern "C"' block */
#endif

#endif /* ifndef _SQLITE_ASSERT_H */
