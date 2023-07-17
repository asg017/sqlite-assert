# sqlite-assert

A SQLite extension for making assertions about your data.

If your company or organization finds this library useful, consider [supporting my work](#supporting)!

## Usage

```sql
.load ./assert0

select assert(1 == 1); -- 1

select assert(1 == 2); -- Fails with "Assertion error"
select assert(1 == 2, '1 does not equal two'); -- Fails with "Assertion error: 1 does not equal two"

select assert_eq(1 + 2, 3); -- 1
select assert_eq("alex", lower("ALEX")); -- 1

select assert_eq(1 + 2, 4); -- Fails with "Assertion error: Value mismatch 3 != 4"
select assert_eq(1, 1.0); -- Fails with "Assertion error: Type mismatch, integer != real"

select assert(created_at between '2022-01-01' and now()) from logs;

select assert(count(*) > 1000) from my_table;

```

## Documentation

See [`docs.md`](./docs.md) for a full API reference.

## Maybe you don't need assertions

`sqlite-assert` can work wonders in certain ETL processes or for 1-off sanity checks - but SQLite does have other validation/constraint features built-in, which you should prefer over `sqlite-assert` in many situations.

### STRICT tables

[SQLite strict tables](https://www.sqlite.org/stricttables.html) were introduced in SQLite version 3.37 (2021-11-27), which offers an alternative to `assert(typeof(value) == 'TYPE')` in many cases.

```sql
create table students(name text, age integer) strict;

insert into students values ('alex', 1); -- ok
insert into students values ('brian', 1+1); -- ok

insert into students values ('craig', 'rogue string');
-- ^ fails with "Runtime error: cannot store TEXT value in INTEGER column students.age (19)"
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
insert into students values ('brian', 20, json_object()); -- fails with "Runtime error: CHECK constraint failed: name assertp '\w+\s\w+' (19)"
insert into students values ('craig', 200, json_object()); -- fails with "Runtime error: CHECK constraint failed: age between 0 and 120 (19)"

```

## Installing

| Language       | Install                                                          |                                                                                                                                                                                                 |
| -------------- | ---------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Python         | `pip install sqlite-assert`                                      | [![PyPI](https://img.shields.io/pypi/v/sqlite-assert.svg?color=blue&logo=python&logoColor=white)](https://pypi.org/project/sqlite-assert/)                                                      |
| Datasette      | `datasette install datasette-sqlite-assert`                      | [![Datasette](https://img.shields.io/pypi/v/datasette-sqlite-assert.svg?color=B6B6D9&label=Datasette+plugin&logoColor=white&logo=python)](https://datasette.io/plugins/datasette-sqlite-assert) |
| Node.js        | `npm install sqlite-assert`                                      | [![npm](https://img.shields.io/npm/v/sqlite-assert.svg?color=green&logo=nodedotjs&logoColor=white)](https://www.npmjs.com/package/sqlite-assert)                                                |
| Deno           | [`deno.land/x/sqlite_assert`](https://deno.land/x/sqlite_assert) | [![deno.land/x release](https://img.shields.io/github/v/release/asg017/sqlite-assert?color=fef8d2&include_prereleases&label=deno.land%2Fx&logo=deno)](https://deno.land/x/sqlite_assert)        |
| Ruby           | `gem install sqlite-assert`                                      | ![Gem](https://img.shields.io/gem/v/sqlite-assert?color=red&logo=rubygems&logoColor=white)                                                                                                      |
| Github Release |                                                                  | ![GitHub tag (latest SemVer pre-release)](https://img.shields.io/github/v/tag/asg017/sqlite-assert?color=lightgrey&include_prereleases&label=Github+release&logo=github)                        |
| Rust           | `cargo add sqlite-assert`                                        | [![Crates.io](https://img.shields.io/crates/v/sqlite-assert?logo=rust)](https://crates.io/crates/sqlite-assert)                                                                                 |
| Go             | `go get -u github.com/asg017/sqlite-assert/bindings/go`          | [![Go Reference](https://pkg.go.dev/badge/github.com/asg017/sqlite-assert/bindings/go.svg)](https://pkg.go.dev/github.com/asg017/sqlite-assert/bindings/go)                                     |

<!--
| Elixir         | [`hex.pm/packages/sqlite_assert`](https://hex.pm/packages/sqlite_assert) | [![Hex.pm](https://img.shields.io/hexpm/v/sqlite_assert?color=purple&logo=elixir)](https://hex.pm/packages/sqlite_assert)                                                                       |
-->

## Supporting

I (Alex 👋🏼) spent a lot of time and energy on this project and [many other open source projects](https://github.com/asg017?tab=repositories&q=&type=&language=&sort=stargazers). If your company or organization uses this library (or you're feeling generous), then please [consider supporting my work](https://alexgarcia.regex/work.html), or share this project with a friend!
