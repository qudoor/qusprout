from db.table import SqlHandle
from db.alltask import TaskHandle
from db.code import CodeHandle
from db.count import CountHandle
from db.elapsed import ElapsedHandle
from db.status import StatusHandle
from db.currtask import CurrTaskHandle
from datetime import datetime, timedelta, timezone
import time

class DbHandle():
    def __init__(self, dbfilename=None):
        self.sql = SqlHandle(dbfilename)
        self.task = TaskHandle(self.sql)
        self.code = CodeHandle(self.sql)
        self.count = CountHandle(self.sql)
        self.elapsed = ElapsedHandle(self.sql)
        self.status = StatusHandle(self.sql)
        self.curr = CurrTaskHandle(self.sql)
        lastTime = datetime.fromtimestamp(time.time(), timezone(timedelta(hours=8)))
        lastTime = lastTime.strftime("%Y-%m-%d")
        self.lastTime = lastTime

    def saveTaskData(self, taskCount, startTime, endTime):
        self.task.savedata(taskCount, startTime, endTime)

    def saveCodeData(self, codeCount, startTime, endTime):
        self.code.savedata(codeCount, startTime, endTime)

    def saveCountData(self, countCount, startTime, endTime):
        self.count.savedata(countCount, startTime, endTime)

    def saveElapsedData(self, elapsedCount, startTime, endTime):
        self.elapsed.savedata(elapsedCount, startTime, endTime)

    def saveStatusData(self, hostname, fixedInfo, dynaInfo, currTime):
        self.status.savedata(hostname, fixedInfo, dynaInfo, currTime)

    def saveCurrTaskData(self, currTaskCount, currTime):
        self.curr.savedata(currTaskCount, currTime)

    def clearData(self):
        cutttime = datetime.fromtimestamp(time.time(), timezone(timedelta(hours=8)))
        cutttime = cutttime.strftime("%Y-%m-%d")
        if cutttime != self.lastTime:
            print("---clearData---")
            self.task.clear(7)
            self.code.clear(7)
            self.count.clear(7)
            self.elapsed.clear(7)
            self.status.clear(7)
            self.curr.clear(7)
            self.lastTime = cutttime

    def selectTaskData(self, startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None):
        return self.task.selectdata(startTimeStr, endTimeStr, hostName, execType, qubitNum)

    def selectCodeData(self, startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None, interfaceName=None, code=None):
        return self.code.selectdata(startTimeStr, endTimeStr, hostName, execType, qubitNum, interfaceName, code)

    def selectCountData(self, startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None, interfaceName=None):
        return self.count.selectdata(startTimeStr, endTimeStr, hostName, execType, qubitNum, interfaceName)

    def selectCurrTaskData(self, startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None):
        return self.curr.selectdata(startTimeStr, endTimeStr, hostName, execType, qubitNum)

    def selectElapsedData(self, startTimeStr=None, endTimeStr=None, hostName=None, execType=None, qubitNum=None, interfaceName=None, elapsed=None):
        return self.elapsed.selectdata(startTimeStr, endTimeStr, hostName, execType, qubitNum, interfaceName, elapsed)

    def selectStatusData(self, startTimeStr=None, endTimeStr=None, hostName=None):
        return self.status.selectdata(startTimeStr, endTimeStr, hostName)

    def selectnewdata(self, startTimeStr=None, endTimeStr=None, hostName=None):
        return self.status.selectnewdata(startTimeStr, endTimeStr, hostName)

    def selectTagList(self, startTimeStr, endTimeStr):
        hostlist = self.status.selecthostlist(startTimeStr, endTimeStr)
        exectypelist = self.count.selectexectypelist(startTimeStr, endTimeStr)
        qubitlist = self.count.selectqubitlist(startTimeStr, endTimeStr)
        interfacenamelist = self.count.selectinterfacenamelist(startTimeStr, endTimeStr)
        elapsedlist = self.elapsed.selectelapsedlist(startTimeStr, endTimeStr)
        codelist = self.code.selectcodelist(startTimeStr, endTimeStr)
        return hostlist, exectypelist, qubitlist, interfacenamelist, elapsedlist, codelist

    def selectAllDataList(self, startTimeStr, endTimeStr, hostName=None, execType=None, qubitNum=None, interfaceName=None):
        taskdata = self.task.selectdata(startTimeStr, endTimeStr, hostName, execType, qubitNum)
        codelist = self.code.selectdata(startTimeStr, endTimeStr, execType, qubitNum, interfaceName, None)
        countlist = self.count.selectdata(startTimeStr, endTimeStr, hostName, execType, qubitNum, interfaceName)
        currlist = self.curr.selectdata(startTimeStr, endTimeStr, hostName, execType, qubitNum)
        elapsedlist = self.elapsed.selectdata(startTimeStr, endTimeStr, hostName, execType, qubitNum, interfaceName, None)
        statuslist = self.status.selectdata(startTimeStr, endTimeStr, hostName)
        return taskdata, codelist, countlist, currlist, elapsedlist, statuslist

    def selectHostInfo(self, hostName=None):
        return self.status.selecthostinfo(hostName)
