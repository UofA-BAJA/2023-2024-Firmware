import sqlite3
import os
from sqlite3 import Error

def create_connection(db_path):
    # Ensure the directory for db_path exists, if not, create it
    db_dir = os.path.dirname(db_path)
    if not os.path.exists(db_dir):
        os.makedirs(db_dir)
    
    conn = None
    try:
        conn = sqlite3.connect(db_path)
        print(f"Connection created at: {db_path}")
        print(f"SQLite version: {sqlite3.version}")
    except Error as e:
        print(e)
    
    return conn

def create_table(conn, create_table_sql):
    try:
        c = conn.cursor()
        c.execute(create_table_sql)
    except Error as e:
        print(e)


def main():
    database_path = os.path.join('db', 'BajaCloud.db') # New path in the 'db' directory
    conn = create_connection(database_path)
    # Add your table creation and other database initialization steps here


if __name__ == "__main__":
    main()
