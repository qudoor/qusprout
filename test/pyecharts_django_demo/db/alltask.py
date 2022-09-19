from db.table import SqlHandle
from datetime import datetime, timedelta, timezone
import time

class TaskHandle():
    def __init__(self, sqlhandle=None):
        self.sqlhandle = sqlhandle
        if self.sqlhandle.isExistTable("SisTaskCount") == False:
            tablesql = '''--任务统计表
            CREATE TABLE SisTaskCount (
                id integer PRIMARY KEY AUTOINCREMENT,		--自增id
                hostName varchar(50) DEFAULT '',			--主机名
                execType int8 DEFAULT 0,					--执行类型 1：cpu 2：cpu+mpi 3：gpu
                qubitNum integer DEFAULT 0,					--qubit数量
                num integer DEFAULT 0,							--任务数
                startTime DATETIME DEFAULT CURRENT_TIMESTAMP,	--开始时间
                endTime DATETIME DEFAULT CURRENT_TIMESTAMP	--结束时间
            );'''
            self.sqlhandle.createTable(tablesql)

    def savedata(self, taskCount, startTime, endTime):
        hostname = taskCount.tag.base.hostname
        if hostname == None:
            hostname = ""
        exectype = taskCount.tag.base.type
        if exectype == None:
            exectype = 0
        qubitnum = taskCount.tag.qubits
        if qubitnum == None:
            qubitnum = 0
        num = taskCount.num
        if num == None:
            num = 0
        if startTime == None:
            startTime = time.time()
        stime = datetime.fromtimestamp(startTime, timezone(timedelta(hours=8)))
        stime = stime.strftime("%Y-%m-%d %H:%M:%S")
        if endTime == None:
            endTime = time.time()
        etime = datetime.fromtimestamp(endTime, timezone(timedelta(hours=8)))
        etime = etime.strftime("%Y-%m-%d %H:%M:%S")
        insertsql = "insert into SisTaskCount(hostName, execType, qubitNum, num, startTime, endTime) values('{}', {}, {}, {}, '{}', '{}');".format(hostname, exectype, qubitnum, num, stime, etime)
        print("savedata machineStatus data(sql:{}).".format(insertsql))
        return self.sqlhandle.update(insertsql)

    def clear(self, leftDay):
        times = time.time() - leftDay*24*60*60
        stime = datetime.fromtimestamp(times, timezone(timedelta(hours=8)))
        stime = etime.strftime("%Y-%m-%d") + " 00:00:00"
        deletesql = "delete from SisTaskCount where endTime < '{}';".format(stime)
        print("clear machineStatus data(sql:{}).".format(deletesql))
        return self.sqlhandle.update(deletesql)

    def selectdata(self, startTimeStr, endTimeStr, hostName, execType, qubitNum):
        selectsql = "select hostName, execType, qubitNum, num, endTime from SisTaskCount where endTime>='{}'".format(startTimeStr)
        if endTimeStr != "" and endTimeStr != None:
            selectsql += " and endTime<='{}'".format(endTimeStr)
        if hostName != None:
            selectsql += " and hostName='{}'".format(hostName)
        if execType != None:
            selectsql += " and execType={}".format(execType)
        if qubitNum != None:
            selectsql += " and qubitNum={}".format(qubitNum)
        selectsql += " order by endTime;"
        rows = self.sqlhandle.select(selectsql)
        return rows
