from db.table import SqlHandle
from datetime import datetime, timedelta, timezone
import time

class ElapsedHandle():
    def __init__(self, sqlhandle=None):
        self.sqlhandle = sqlhandle
        if self.sqlhandle.isExistTable("SisInterfaceElapsed") == False:
            tablesql = '''--接口调用耗时统计表
            CREATE TABLE SisInterfaceElapsed (
                id integer PRIMARY KEY AUTOINCREMENT,		--自增id
                hostName varchar(50) DEFAULT '',			--主机名
                execType int8 DEFAULT 0,					--执行类型 1：cpu 2：cpu+mpi 3：gpu
                qubitNum integer DEFAULT 0,					--qubit数量
                interfaceName varchar(50) DEFAULT '',		--接口名称
                elapsed integer DEFAULT 0,					--耗时，单位：ms
                count integer DEFAULT 0,					--次数
                startTime DATETIME DEFAULT CURRENT_TIMESTAMP,	--开始时间
                endTime DATETIME DEFAULT CURRENT_TIMESTAMP	--结束时间
            );'''
            self.sqlhandle.createTable(tablesql)

    def savedata(self, elapsedCount, startTime, endTime):
        hostname = elapsedCount.tag.base.hostname
        if hostname == None:
            hostname = ""
        exectype = elapsedCount.tag.base.type
        if exectype == None:
            exectype = 0
        qubitnum = elapsedCount.tag.qubits
        if qubitnum == None:
            qubitnum = 0
        interface_name = elapsedCount.tag.interface_name
        if interface_name == None:
            interface_name = 0
        elapsed = elapsedCount.elapsed
        if elapsed == None:
            elapsed = 0
        count = elapsedCount.count
        if count == None:
            count = 0
        if startTime == None:
            startTime = time.time()
        stime = datetime.fromtimestamp(startTime, timezone(timedelta(hours=8)))
        stime = stime.strftime("%Y-%m-%d %H:%M:%S")
        if endTime == None:
            endTime = time.time()
        etime = datetime.fromtimestamp(endTime, timezone(timedelta(hours=8)))
        etime = etime.strftime("%Y-%m-%d %H:%M:%S")
        insertsql = "insert into SisInterfaceElapsed(hostName, execType, qubitNum, interfaceName, elapsed, count, startTime, endTime) values('{}', {}, {}, '{}', {}, {}, '{}', '{}');".format(hostname, exectype, qubitnum, interface_name, elapsed, count, stime, etime)
        print("savedata SisInterfaceElapsed data(sql:{}).".format(insertsql))
        return self.sqlhandle.update(insertsql)

    def clear(self, leftDay):
        times = time.time() - leftDay*24*60*60
        stime = datetime.fromtimestamp(times, timezone(timedelta(hours=8)))
        stime = etime.strftime("%Y-%m-%d") + " 00:00:00"
        deletesql = "delete from SisInterfaceElapsed where endTime < '{}';".format(stime)
        print("clear SisInterfaceElapsed data(sql:{}).".format(deletesql))
        return self.sqlhandle.update(deletesql)

    def selectdata(self, startTimeStr, endTimeStr, hostName, execType, qubitNum, interfaceName, elapsed):
        selectsql = "select hostName, execType, qubitNum, interfaceName, elapsed, count, endTime from SisInterfaceElapsed where endTime>='{}'".format(startTimeStr)
        if endTimeStr != "" and endTimeStr != None:
            selectsql += " and endTime<='{}'".format(endTimeStr)
        if hostName != None:
            selectsql += " and hostName='{}'".format(hostName)
        if execType != None:
            selectsql += " and execType={}".format(execType)
        if qubitNum != None:
            selectsql += " and qubitNum={}".format(qubitNum)
        if interfaceName != None:
            selectsql += " and interfaceName='{}'".format(interfaceName)
        if elapsed != None:
            selectsql += " and elapsed={}".format(elapsed)
        selectsql += " order by endTime;"
        rows = self.sqlhandle.select(selectsql)
        return rows

    def selectelapsedlist(self, startTimeStr, endTimeStr):
        selectsql = "select elapsed from SisInterfaceElapsed where 1=1"
        if startTimeStr != "" and startTimeStr != None:
            selectsql += " and startTime>='{}'".format(startTimeStr)
        if endTimeStr != "" and endTimeStr != None:
            selectsql += " and endTime<='{}'".format(endTimeStr)
        selectsql += " group by elapsed;"
        rows = self.sqlhandle.select(selectsql)
        return rows