```sql
select
  name,
  assert_group_size()
from data;
```

## sqlite-assert

Make asserts about your data in SQLite

## Usage

## Usage

```sql
.load ./assert0
select assert(0 == 1); -- Fails with ""
select assert(0 == 1, ''); -- Fails with ""


select assert(created_at between '2022-01-01' and now());

select assert(count(*) > 1000) from table;

select assert()
```

## Documentation

See [`docs.md`](./docs.md) for a full API reference.

## Maybe you don't need assertions

`sqlite-assert` can work wonders in certain ETL processes or for 1-off sanity checks - but SQLite does have other validation/constraint features builtin, for tables and column definitions.

### STRICT tables

[SQLite strict tables](https://www.sqlite.org/stricttables.html) were introduced in SQLite version 3.37.0 (2021-11-27), which offers an alternative to `asser_type()` and `assert(typeof(value) == 'TYPE')` in some cases.

```sql
create table students(name text, age integer) strict;

insert into students values ('alex', 1); -- ok
insert into students values ('brian', 1+1); -- ok
insert into students values ('craig', 'rogue string'); -- fails with "Runtime error: cannot store TEXT value in INTEGER column students.age (19)"
```

### CHECK Column Constraints

In `CREATE TABLE` statements, you can define [`CHECK` constraints](https://www.sqlite.org/lang_createtable.html#check_constraints) to ensure column values follow any set of rules.

```sql
create table students(
  name text check(name regexp '\w+\s\w+'),
  age integer check(age between 0 and 120),
  data json check(json_valid(data) and json_type(data) == 'object')
);

insert into students values ('alex garcia', 10, json_object()); -- ok
insert into students values ('brian', 20, json_object()); -- fails with "Runtime error: CHECK constraint failed: name regexp '\w+\s\w+' (19)"
insert into students values ('craig craig', 200, json_object()); -- fails with "Runtime error: CHECK constraint failed: age between 0 and 120 (19)"

```

## Installing

TODO

### As a loadable extension

If you want to use `sqlite-assert` as a [Runtime-loadable extension](https://www.sqlite.org/loadext.html), Download the `assert0.dylib` (for MacOS), `assert0.so` (Linux), or `assert0.dll` (Windows) file from a release and load it into your SQLite environment.

> **Note:**
> The `0` in the filename (`assert0.dylib`/ `assert0.so`/`assert0.dll`) denotes the major version of `sqlite-path`. Currently `sqlite-path` is pre v1, so expect breaking changes in future versions.

For example, if you are using the [SQLite CLI](https://www.sqlite.org/cli.html), you can load the library like so:

```sql
.load ./assert0
select assert_version();
-- v0.0.1
```

Or in Python, using the builtin [sqlite3 module](https://docs.python.org/3/library/sqlite3.html):

```python
import sqlite3

con = sqlite3.connect(":memory:")

con.enable_load_extension(True)
con.load_extension("./assert0")

print(con.execute("select assert_version()").fetchone())
# ('v0.0.1',)
```

Or in Node.js using [better-sqlite3](https://github.com/WiseLibs/better-sqlite3):

```javascript
const Database = require("better-sqlite3");
const db = new Database(":memory:");

db.loadExtension("./assert0");

console.log(db.prepare("select assert_version()").get());
// { 'html_version()': 'v0.0.1' }
```

Or with [Datasette](https://datasette.io/):

```
datasette data.db --load-extension ./assert0
```

## See also

- [sqlite-url](https://github.com/asg017/sqlite-url), for parsing and generating URLs (pairs well with this library)
- [sqlite-http](https://github.com/asg017/sqlite-http), for making HTTP requests in SQLite
- [sqlite-html](https://github.com/asg017/sqlite-html), for parsing HTML documents
- [sqlite-lines](https://github.com/asg017/sqlite-lines), for reading large files line-by-line
- [nalgeon/sqlean](https://github.com/nalgeon/sqlean), several pre-compiled handy SQLite functions, in C
  inspiro:

- python [unittest](https://docs.python.org/3/library/unittest.html)
- deno [assertions](https://deno.land/manual/testing/assertions)

- [x] `assert(d, msg)`
- [ ] `assert_equal(a, b, msg)`
- [ ] `assert_notequal(a, b, msg)`
- [ ] `assert_null(d, msg)`
- [ ] `assert_notnull(d, msg)`
- [ ] `assert_type(d, type, msg)`
- [ ] `assert_subtype(d, subtype, msg)`
