import git
import sqlite3
import os
from sqlite3 import Error

def find_git_root(path='.'):
    try:
        # This will find the root directory of the git repo
        git_repo = git.Repo(path, search_parent_directories=True)
        git_root = git_repo.git.rev_parse("--show-toplevel")
        return git_root
    except git.exc.InvalidGitRepositoryError:
        print("Error: Specified directory is not a Git repository.")
        return None

def get_BajaCloud_connection():
    git_root = find_git_root()

    data_db_path = os.path.join(git_root, 'src', 'Controller - Pi', 'db', 'BajaCloud.db') # New path in the 'db' directory

    conn = create_connection(data_db_path)
    # Add your table creation and other database initialization steps here
    if conn is not None:
        # You can create tables here using conn
        # For example: create_table(conn, sql_create_my_table)
        # print(f"Database created at: {data_db_path}")
        # Don't forget to close the connection when done
        conn.close()
    else:
        print("Error! Cannot create the database connection.")

def create_connection(db_path):
    # Ensure the directory for db_path exists, if not, create it
    db_dir = os.path.dirname(db_path)
    if not os.path.exists(db_dir):
        os.makedirs(db_dir)
    
    conn = None
    try:
        conn = sqlite3.connect(db_path)
        # print(f"Connection created at: {db_path}")
        # print(f"SQLite version: {sqlite3.version}")
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
    git_root = find_git_root()

    data_db_path = os.path.join(git_root, 'src', 'Controller - Pi', 'db', 'BajaCloud.db') # New path in the 'db' directory
    data_db_sql = '''
    CREATE TABLE IF NOT EXISTS BajaCloudData (
        ID INTEGER,
        Time REAL,
        IMU REAL,
        RPM_REAR_LEFT REAL,
        RPM_REAR_RIGHT REAL,
        RPM_FRONT_LEFT REAL,
        RPM_FRONT_RIGHT REAL,
        BRAKE_PRESSURE REAL,
        GAS_ANGLE REAL,
        BRAKE_ANGLE REAL,
        ENGINE_RPM REAL,
        FOREIGN KEY (ID) REFERENCES BajaCloudSessions (ID)
    );'''

    conn = create_connection(data_db_path)
    # Add your table creation and other database initialization steps here
    if conn is not None:
        # You can create tables here using conn
        create_table(conn, data_db_sql)
        # print(f"Database created at: {data_db_path}")
        # Don't forget to close the connection when done
        conn.close()
    else:
        print("Error! Cannot create the database connection.")


if __name__ == "__main__":
    main()
