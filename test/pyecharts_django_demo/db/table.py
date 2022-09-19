import sqlite3

class SqlHandle():
    def __init__(self, dbfilename=None):
        self.conn = sqlite3.connect(dbfilename, timeout=30, check_same_thread=False)
        self.cur = self.conn.cursor()
        self._time_now = "[" + sqlite3.datetime.datetime.now().strftime('%Y/%m/%d %H:%M:%S') + "]"

    def createTable(self, createsql=None):
        try:
            self.cur.execute(createsql)
            self.conn.commit()
            return True
        except Exception as e:
            print(self._time_now, " CREATE TABLE ERROR(err:", e, ",sql:", createsql,  ").")
            return False


    def update(self, updatesql=None):
        try:
            self.cur.execute(updatesql)
            self.conn.commit()
            return True
        except Exception as e:
            print(self._time_now, " CREATE TABLE ERROR(err:", e, ",sql:", updatesql,  ").")
            return False

    def select(self, selectsql=None):
        try:
            self.cur.execute(selectsql)
            self.conn.commit()
            tabledata = self.cur.fetchall()
            return tabledata
        except Exception as e:
            print(self._time_now, " CREATE TABLE ERROR(err:", e, ",sql:", selectsql,  ").")
            return None

    def isExistTable(self, tablename=None):
        sql = "SELECT count(1) FROM sqlite_master WHERE type='table' AND name='" + tablename + "' COLLATE NOCASE"
        rows = self.select(sql)
        for row in rows:
            if row[0] == 1:
                return True
        return False

    def close(self):
        try:
            self.cur.close()
            self.conn.close()
            return True
        except Exception as e:
            print(self._time_now, "[CREATE TABLE ERROR]", e)
            return False