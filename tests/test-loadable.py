import sqlite3
import unittest

EXT_PATH = "dist/assert0"

def connect():
  db = sqlite3.connect(":memory:")

  db.execute("create table fbefore as select name from pragma_function_list")
  db.execute("create table mbefore as select name from pragma_module_list")

  db.enable_load_extension(True)
  db.load_extension(EXT_PATH)

  db.execute("create temp table fafter as select name from pragma_function_list")
  db.execute("create temp table mafter as select name from pragma_module_list")

  return db

db = connect()

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
    funcs = list(map(lambda a: a[0], db.execute("select name from fafter where name not in (select name from fbefore) order by name").fetchall()))
    self.assertEqual(funcs, [
      "assert",
      "assert",
      "assert_equal",
      "assert_equal",
      "assert_notequal",
      "assert_notequal",
      "assert_notnull",
      "assert_notnull",
      "assert_null",
      "assert_null",
      "assert_subtype",
      "assert_subtype",
      "assert_type",
      "assert_type",
    ])

  def test_assert(self):
    self.assertEqual(exec("select assert(1 == 1)"), 1)

    with self.assertRaisesRegex(sqlite3.OperationalError, 'Assertion error'):
      exec("select assert(1 == 2)")
    
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: one equals two"):
      exec("select assert(1 == 2, 'one equals two')")
    #self.assertEqual('abc', '''abcd
    #asdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfasdf''')

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
    with self.assertRaisesRegex(sqlite3.OperationalError,  "Assertion error: msg"):
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
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: 1 != 2"):
      exec("select assert_equal(1, 2)")
    with self.assertRaisesRegex(sqlite3.OperationalError,  "Assertion error: 1.010000 != 1.000000"):
      exec("select assert_equal(1.01, 1.0)")
    with self.assertRaisesRegex(sqlite3.OperationalError, 'Assertion error: "abc" != "abb"'):
      exec("select assert_equal('abc', 'abb')")
    with self.assertRaisesRegex(sqlite3.OperationalError, 'Assertion error: "abc" != "abb"'):
      exec("select assert_equal(cast('abc' as blob), cast('abb' as blob))")
    #with self.assertRaisesRegex(sqlite3.OperationalError, 'Assertion error: "abc" != "abb"'):
      exec("select assert_equal('abc', 'abbb')")
    
    #self.assertEqual(exec("select assert_equal(1, 1)"), 1)
    #with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error"):
    #  exec("select assert_equal(1, 2)")
    #with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: one equals two"): 
    #  exec("select assert_equal(1, 2, 'one equals two')")

  def test_assert_notequal(self):
    self.assertEqual(exec("select assert_notequal(1  1)"), 1)

    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error"):
      exec("select assert(1 == 2)")
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: one equals two"):
      exec("select assert(1 == 2, 'one equals two')")
    
  def test_assert_null(self):
    self.assertEqual(exec("select assert_null(null)"), 1)
    self.assertEqual(exec("select assert_null(nullif(1,1))"), 1)
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error, value not not null"):
      exec("select assert_null('')")


  def test_assert_notnull(self):
    self.skipTest("TODO")
    self.assertEqual(exec("select assert(1 == 1)"), 1)
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error"):
      exec("select assert(1 == 2)")
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: one equals two"):
      exec("select assert(1 == 2, 'one equals two')")

  def test_assert_type(self):
    self.skipTest("TODO")
    self.assertEqual(exec("select assert(1 == 1)"), 1)
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error"):
      exec("select assert(1 == 2)")
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: one equals two"):
      exec("select assert(1 == 2, 'one equals two')")

  def test_assert_subtype(self):
    self.skipTest("TODO")
    self.assertEqual(exec("select assert(1 == 1)"), 1)
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error"):
      exec("select assert(1 == 2)")
    with self.assertRaisesRegex(sqlite3.OperationalError, "Assertion error: one equals two"):
      exec("select assert(1 == 2, 'one equals two')")
  
  
if __name__ == '__main__':
  unittest.main()
