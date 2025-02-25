# sqlite-assert Documentation

A full reference to every function that sqlite-assert offers.

As a reminder, sqlite-assert follows semver and is pre v1, so breaking changes are to be expected.

## API Reference

### `assert_version()`
Returns the semver version string of the current version of sqlite-assert.

```sql
select assert_version();
-- "v0.0.0"
```

### `assert_debug()`
Returns a debug string containing information about sqlite-assert, including version, build date, commit hash, and cwalk version.

```sql
select assert_debug();
/*
Version: v0.0.0
Date: 2022-08-19T17:27:14Z-0700
Source: 01cd76716130b739f3e33177740e92e7ad0cff35
cwalk version: v1.2.6
*/
```

### `assert(value [, message])`
Throws an error if the provided value evaluates to `0` or `FALSE`. Returns `1` if the assertion passes.

- **Parameters**:
  - `value`: The boolean expression to evaluate.
  - `message`: Optional custom error message.

```sql
select assert(1); -- 1
select assert(1 == 1); -- 1
select assert(1 == 2); -- Fails with "Assertion error"
select assert(1 == 2, 'One does not equal two'); -- Fails with "Assertion error: One does not equal two"
```

### `assert_eq(value1, value2 [, message])`
Asserts that `value1` equals `value2`. Returns `1` if they are equal. Throws an error with a detailed message if they are not.

- **Parameters**:
  - `value1`: The first value to compare.
  - `value2`: The second value to compare.
  - `message`: Optional custom error message.

```sql
select assert_eq(1 + 2, 3); -- 1
select assert_eq("alex", lower("ALEX")); -- 1
select assert_eq(1 + 2, 4); -- Fails with "Assertion error: Value mismatch 3 != 4"
select assert_eq(1, 1.0); -- Fails with "Assertion error: Type mismatch, integer != real"
select assert_eq(' hello', 'Hello', 'Strings do not match'); 
-- Fails with "Assertion error: Value mismatch " hello" != "Hello" - Strings do not match"
```
