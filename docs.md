# sqlite-assert Documentation

A full reference to every function and module that sqlite-assert offers.

As a reminder, sqlite-assert follows semver and is pre v1, so breaking changes are to be expected.

## API Reference

<h3 name=assert_version> <code>assert_version()</code></h3>

Returns the semver version string of the current version of sqlite-assert.

```sql
select assert_version();
-- "v0.0.0"
```

<h3 name=assert_debug> <code>assert_debug()</code></h3>

Returns a debug string of various info about sqlite-assert, including
the version string, build date, commit hash, and cwalk version.

```sql
select assert_debug();
/*
Version: v0.0.0
Date: 2022-08-19T17:27:14Z-0700
Source: 01cd76716130b739f3e33177740e92e7ad0cff35
cwalk version: v1.2.6
*/
```

<h3 name=assert> <code>assert(value [, message])</code></h3>

If value evaluates to `1` or `TRUE`, then `1` is returned. Otherwise, an error is thrown with the message `"Assertion error"`.

If the assertion fails and `message` is passed in, then the error message will read `"Assertion error: {message}"`.

```sql
select assert(1); -- 1
select assert((1 + 1) == 2); -- 1
select assert((1 + 1) == 3); -- Fails with "Assertion error"
select assert((1 + 1) == 3, 'One does not equal three'); -- Fails with "Assertion error: One does not equal three"
select assert((1 + 1) == 2); -- 1
```

<h3 name=assert_equals> <code>assert_equals(value1, value2 [, message])</code></h3>

Returns 1 if the given path is absolute, 0 otherwise.

```sql
select assert_("/usr/local/bin"); -- 1
select assert_("./rel/to/me"); -- 0
```

<h3 name=assert_type> <code>assert_type(path, type)</code></h3>

Returns 1 if the given path is absolute, 0 otherwise.

```sql
select assert_("/usr/local/bin"); -- 1
select assert_("./rel/to/me"); -- 0
```

<h3 name=assert_subtype> <code>assert_subtype(path, subtype)</code></h3>

Returns 1 if the given path is absolute, 0 otherwise.

```sql
select assert_("/usr/local/bin"); -- 1
select assert_("./rel/to/me"); -- 0
```
