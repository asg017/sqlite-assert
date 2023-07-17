from datasette import hookimpl
import sqlite_assert

from datasette_sqlite_assert.version import __version_info__, __version__

@hookimpl
def prepare_connection(conn):
    conn.enable_load_extension(True)
    sqlite_assert.load(conn)
    conn.enable_load_extension(False)
