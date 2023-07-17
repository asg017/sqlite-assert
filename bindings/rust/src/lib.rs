#[cfg(feature = "assert")]
#[link(name = "sqlite_assert0")]
extern "C" {
    pub fn sqlite3_assert_init();
}

#[cfg(test)]
mod tests {
    use super::*;

    use rusqlite::{ffi::sqlite3_auto_extension, Connection};

    #[test]
    fn test_rusqlite_auto_extension() {
        unsafe {
            sqlite3_auto_extension(Some(sqlite3_assert_init));
        }

        let conn = Connection::open_in_memory().unwrap();

        let result: String = conn
            .query_row("select assert(?)", ["alex"], |x| x.get(0))
            .unwrap();

        assert_eq!(result, "Assert, alex!");
    }
}
