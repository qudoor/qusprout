from db.table import SqlHandle
from datetime import datetime, timedelta, timezone
import time

class CurrTaskHandle():
    def __init__(self, sqlhandle=None):
        self.sqlhandle = sqlhandle
        if self.sqlhandle.isExistTable("SisCurrTaskCount") == False:
            tablesql = '''--当前任务统计表
            CREATE TABLE SisCurrTaskCount (
                id integer PRIMARY KEY AUTOINCREMENT,			--自增id
                hostName varchar(50) DEFAULT '',				--主机名
                execType int8 DEFAULT 0,						--执行类型 1：cpu 2：cpu+mpi 3：gpu
                qubitNum integer DEFAULT 0,						--qubit数量
                currTaskNum integer DEFAULT 0,					--当前任务数
                currTimeoutTaskNum integer DEFAULT 0,			--当前超过1分钟没有更新的任务数
                currTime DATETIME DEFAULT CURRENT_TIMESTAMP	    --当前时间
            );'''
            self.sqlhandle.createTable(tablesql)

    def savedata(self, currtaskCount, currTime):
        hostname = currtaskCount.tag.base.hostname
        if hostname == None:
            hostname = ""
        exectype = currtaskCount.tag.base.type
        if exectype == None:
            exectype = 0
        qubitnum = currtaskCount.tag.qubits
        if qubitnum == None:
            qubitnum = 0
        num = currtaskCount.num
        if num == None:
            num = 0
        timeout_num = currtaskCount.timeout_num
        if timeout_num == None:
            timeout_num = 0
        if currTime == None:
            currTime = time.time()
        ctime = datetime.fromtimestamp(currTime, timezone(timedelta(hours=8)))
        ctime = ctime.strftime("%Y-%m-%d %H:%M:%S")
        insertsql = "insert into SisCurrTaskCount(hostName, execType, qubitNum, currTaskNum, currTimeoutTaskNum, currTime) values('{}', {}, {}, {}, {}, '{}');".format(hostname, exectype, qubitnum, num, timeout_num, ctime)
        print("savedata SisCurrTaskCount data(sql:{}).".format(insertsql))
        return self.sqlhandle.update(insertsql)

    def clear(self, leftDay):
        times = time.time() - leftDay*24*60*60
        stime = datetime.fromtimestamp(times, timezone(timedelta(hours=8)))
        stime = etime.strftime("%Y-%m-%d") + " 00:00:00"
        deletesql = "delete from SisCurrTaskCount where currTime < '{}';".format(stime)
        print("clear SisCurrTaskCount data(sql:{}).".format(deletesql))
        return self.sqlhandle.update(deletesql)

    def selectdata(self, startTimeStr, endTimeStr, hostName, execType, qubitNum):
        selectsql = "select hostName, execType, qubitNum, currTaskNum, currTimeoutTaskNum, currTime from SisCurrTaskCount where currTime>='{}'".format(startTimeStr)
        if endTimeStr != "" and endTimeStr != None:
            selectsql += " and currTime<='{}'".format(endTimeStr)
        if hostName != None:
            selectsql += " and hostName='{}'".format(hostName)
        if execType != None:
            selectsql += " and execType={}".format(execType)
        if qubitNum != None:
            selectsql += " and qubitNum={}".format(qubitNum)
        selectsql += " order by currTime;"
        rows = self.sqlhandle.select(selectsql)
        return rows