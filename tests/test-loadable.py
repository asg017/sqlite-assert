import sqlite3
import unittest

EXT_PATH = "dist/assert0"

FUNCTIONS = [
  "assert",
  "assert_equal",
]

MODULES = []


def connect(ext):
  db = sqlite3.connect(":memory:")

  db.execute("create table base_functions as select name from pragma_function_list")
  db.execute("create table base_modules as select name from pragma_module_list")

  db.enable_load_extension(True)
  db.load_extension(ext)

  db.execute("create temp table loaded_functions as select name from pragma_function_list where name not in (select name from base_functions) group by 1 order by name")
  db.execute("create temp table loaded_modules as select name from pragma_module_list where name not in (select name from base_modules) group by 1 order by name")

  db.row_factory = sqlite3.Row
  return db

db = connect(EXT_PATH)

def exec(query):
  return db.execute(query).fetchone()[0]

# if returns None, then no error was raised or it wasnt an OperationalError
def exec_raises(query):
  try:
    result = exec(query)
    return None
  except sqlite3.OperationalError as e:
    return str(e)
  except Exception:
    return None

class TestAssert(unittest.TestCase):
  def test_funcs(self):
    funcs = list(map(lambda a: a[0], db.execute("select name from loaded_functions").fetchall()))
    self.assertEqual(funcs, FUNCTIONS)

  def test_modules(self):
    modules = list(map(lambda a: a[0], db.execute("select name from loaded_modules").fetchall()))
    self.assertEqual(modules, MODULES)


  def test_assert(self):
    self.assertEqual(exec("select assert(1 == 1)"), 1)
    self.assertEqual(exec("select assert(1 + 1 == 2)"), 1)
    self.assertEqual(exec("select assert('abc ')"), 1)

    with self.assertRaisesRegex(sqlite3.OperationalError, 'Assertion error'):
      exec("select assert(1 == 2)")

    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: one equals two"):
      exec("select assert(1 == 2, 'one equals two')")

  # Assertion error:  1 != 2
  # Assertion error:  2 != "2"
  # Assertion error:  2 != 2.0
  # Assertion error:  2 != Blob(n=2)
  # Assertion error:  2 != "abc\nxyz"
  # Assertion error:  2 != 'abcd\n    asdfasdfasdfasdfasdfasdfasdfasdfas[138 chars]asdf'
  def test_assert_equal(self):
    self.assertEqual(exec("select assert_equal(null, null)"), 1)
    self.assertEqual(exec("select assert_equal(1, 1)"), 1)
    self.assertEqual(exec("select assert_equal(1.0, 1.0)"), 1)
    self.assertEqual(exec("select assert_equal('str', 'str')"), 1)
    self.assertEqual(exec("select assert_equal(zeroblob(10), zeroblob(10))"), 1)
    self.assertEqual(exec("select assert_equal(cast('abc' as blob), cast('abc' as blob))"), 1)

    with self.assertRaisesRegex(sqlite3.OperationalError,  "Assertion error: Type mismatch, integer != text - msg"):
      exec("select assert_equal(1, 'a', 'msg')")

    # type mismatches
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: Type mismatch, integer != text"):
      exec("select assert_equal(1, 'a')")

    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: Type mismatch, text != integer"):
      exec("select assert_equal('a', 2)")

    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: Type mismatch, blob != integer"):
      exec("select assert_equal(zeroblob(1), 1)")

    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: Type mismatch, null != integer"):
      exec("select assert_equal(null, 1)")

    # value mismatches
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: Value mismatch 1 != 2"):
      exec("select assert_equal(1, 2)")

    with self.assertRaisesRegex(sqlite3.OperationalError,  "Assertion error: Value mismatch 1.010000 != 1.000000"):
      exec("select assert_equal(1.01, 1.0)")

    with self.assertRaisesRegex(sqlite3.OperationalError, 'Assertion error: Value mismatch "abc" != "abb"'):
      exec("select assert_equal('abc', 'abb')")

    with self.assertRaisesRegex(sqlite3.OperationalError, 'Assertion error: Value mismatch "abc" != "abb"'):
      exec("select assert_equal(cast('abc' as blob), cast('abb' as blob))")

   # Length mismatches
    with self.assertRaisesRegex(sqlite3.OperationalError, 'Assertion error: Text length mismatch, 3 != 4'):
      exec("select assert_equal('abc', 'abbb')")

    with self.assertRaisesRegex(sqlite3.OperationalError, 'Assertion error: Blob length mismatch, 3 != 4'):
      exec("select assert_equal(cast('abc' as blob), cast('abbb' as blob))")

    #self.assertEqual(exec("select assert_equal(1, 1)"), 1)
    #with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error"):
    #  exec("select assert_equal(1, 2)")
    #with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: one equals two"):
    #  exec("select assert_equal(1, 2, 'one equals two')")

if __name__ == '__main__':
  unittest.main()
