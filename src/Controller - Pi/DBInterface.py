import sqlite3

from sqlite3 import Error


def create_connection(db_path):
    conn = None

    try:
        conn = sqlite3.connect(db_path)
        print("Connection created at: " + db_path)
        print("SQLite version: " + sqlite3.version)
    except Error as e:
        print(e)
    
    return conn

def create_table(conn, create_table_sql):
    """ create a table from the create_table_sql statement
    :param conn: Connection object
    :param create_table_sql: a CREATE TABLE statement
    :return:
    """
    try:
        c = conn.cursor()
        c.execute(create_table_sql)
    except Error as e:
        print(e)


def main():
    conn = create_connection('pythonsqlite.db')


if __name__ == "__main__":
    main()