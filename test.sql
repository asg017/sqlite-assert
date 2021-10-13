.bail on
.load assert

select assert(1+1 == 2, "Arithmetic.");

create temp table evens as
  select value from generate_series(0, 20, 2);

create temp view evens_assert as
  select
    assert(value % 2 == 0, "events.value should be even")
  from evens;

select assert_query('select * from evens_assert;');


create temp table odds as
  select value from generate_series(1, 21, 3); --  a bug!

create temp view odds_assert as
  select assert(value % 2 == 1, "odds.value should be odd") as "odd"
  from odds;

select assert_query('select * from odds_assert;');
--select assert_view('odds_assert;');

.exit 0