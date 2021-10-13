# sqlite-assert

```
gcc -g -I. -fPIC -dynamiclib  assert.c -o assert.dylib
./sqlite3 -init test.sql
```

codebase is a mess
https://sqlite.org/forum/forumpost/9696e104ae7badcfe6d980d76a98823c6e5cc6504b43326849b7979a5bb3b58a

```sql
.load assert

select assert(1+1 == 2, "Arithmetic.");

create temp table evens as
  select value from generate_series(0, 20, 2);

select
  assert(value % 2 == 0, "events.value should be even")
from events;

create temp table odds as
  select value from generate_series(1, 21, 2);

create temp view odds_assert as
  select assert(value % 2 == 1, "odds.value should be odd") as "odd"
  from odds;

select assert_query('select * from odds_assert;');
select assert_view('odds_assert;');
```
