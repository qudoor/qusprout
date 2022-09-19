from db.table import SqlHandle
from datetime import datetime, timedelta, timezone
import time

class StatusHandle():
    def __init__(self, sqlhandle=None):
        self.sqlhandle = sqlhandle
        if self.sqlhandle.isExistTable("machineStatus") == False:
            tablesql = '''--机器状态表
            CREATE TABLE machineStatus (
                id integer PRIMARY KEY AUTOINCREMENT,		--自增id
                hostName varchar(50) DEFAULT '',			--主机名
                allCpuNum integer DEFAULT 0,				--cpu数量
                allMemory integer DEFAULT 0,				--内存
                useMemory integer DEFAULT 0,				--已用内存
                gpuType int8 DEFAULT 0,						--gpu类型 1：nvidia
                allGpuMemory integer DEFAULT 0,				--gpu内存
                useGpuMemory integer DEFAULT 0,				--已用gpu内存
                currTime DATETIME DEFAULT CURRENT_TIMESTAMP	--当前时间
            );'''
            self.sqlhandle.createTable(tablesql)

    def savedata(self, hostname, fixedInfo, dynaInfo, currTime):
        hname = hostname
        if hname == None:
            hname = ""
        cpu_num = fixedInfo.cpu_num
        if cpu_num == None:
            cpu_num = 0
        cpu_memory = fixedInfo.cpu_memory
        if cpu_memory == None:
            cpu_memory = 0
        cpu_used_memory = dynaInfo.cpu_used_memory
        if cpu_used_memory == None:
            cpu_used_memory = 0
        gpu_type = fixedInfo.gpu_type
        if gpu_type == None:
            gpu_type = 0
        gpu_memory = fixedInfo.gpu_memory
        if gpu_memory == None:
            gpu_memory = 0
        gpu_used_memory = dynaInfo.gpu_used_memory
        if gpu_used_memory == None:
            gpu_used_memory = 0
        if currTime == None:
            currTime = time.time()
        ctime = datetime.fromtimestamp(currTime, timezone(timedelta(hours=8)))
        ctime = ctime.strftime("%Y-%m-%d %H:%M:%S")
        insertsql = "insert into machineStatus(hostName, allCpuNum, allMemory, useMemory, gpuType, allGpuMemory, useGpuMemory, currTime) values('{}', {}, {}, {}, {}, {}, {}, '{}');".format(hname, cpu_num, cpu_memory, cpu_used_memory, gpu_type, gpu_memory, gpu_used_memory, ctime)
        print("savedata machineStatus data(sql:{}).".format(insertsql))
        return self.sqlhandle.update(insertsql)

    def clear(self, leftDay):
        times = time.time() - leftDay*24*60*60
        stime = datetime.fromtimestamp(times, timezone(timedelta(hours=8)))
        stime = stime.strftime("%Y-%m-%d") + " 00:00:00"
        deletesql = "delete from machineStatus where currTime < '{}';".format(stime)
        print("clear machineStatus data(sql:{}).".format(deletesql))
        return self.sqlhandle.update(deletesql)

    def selectdata(self, startTimeStr, endTimeStr, hostName):
        selectsql = "select hostName, allCpuNum, allMemory, useMemory/1024.0/1024.0, gpuType, allGpuMemory, useGpuMemory, currTime from machineStatus where currTime>='{}'".format(startTimeStr)
        if endTimeStr != "" and endTimeStr != None:
            selectsql += " and currTime<='{}'".format(endTimeStr)
        if hostName != None:
            selectsql += " and hostName='{}'".format(hostName)
        selectsql += " order by currTime;"
        rows = self.sqlhandle.select(selectsql)
        return rows

    def selectnewdata(self, startTimeStr, endTimeStr, hostName):
        selectsql = "select hostName, allCpuNum, allMemory, gpuType, allGpuMemory from machineStatus where currTime>='{}'".format(startTimeStr)
        if endTimeStr != "" and endTimeStr != None:
            selectsql += " and currTime<='{}'".format(endTimeStr)
        if hostName != None:
            selectsql += " and hostName='{}'".format(hostName)
        selectsql += " group by hostName;"
        rows = self.sqlhandle.select(selectsql)
        return rows

    def selecthostlist(self, startTimeStr, endTimeStr):
        selectsql = "select hostName from machineStatus where 1=1"
        if startTimeStr != "" and startTimeStr != None:
            selectsql += " and currTime>='{}'".format(startTimeStr)
        if endTimeStr != "" and endTimeStr != None:
            selectsql += " and currTime<='{}'".format(endTimeStr)
        selectsql += " group by hostName;"
        rows = self.sqlhandle.select(selectsql)
        return rows

    def selecthostinfo(self, hostname):
        selectsql = "select hostName, allCpuNum, allMemory, gpuType, allGpuMemory from machineStatus where 1=1"
        if hostname != None:
            selectsql += " and hostName='{}'".format(hostname)
        selectsql += " group by hostName;"
        rows = self.sqlhandle.select(selectsql)
        return rows