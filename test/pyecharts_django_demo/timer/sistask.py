from db.db import DbHandle
from qusprout.rpcclient import ResourceServer
from timer.timer import TimerHandle
from qusprout.code.ttypes import ErrCode

class StatisticsTask():
    def __init__(self):
        self.db = DbHandle("a.db")
        self.resource = ResourceServer('127.0.0.1', 9092)

    def syncStatisticsToDb(self):
        resp = self.resource.gettatistics()
        if resp.base.code != ErrCode.COM_SUCCESS:
            print("gettatistics failed(resp:{}).".format(resp))
            return
        for hostname, sis in resp.sis_list.items():
            for taskcount in sis.all_task_num.values():
                self.db.saveTaskData(taskcount, sis.begin_time, sis.end_time)
            for codecount in sis.code_count.values():
                self.db.saveCodeData(codecount, sis.begin_time, sis.end_time)
            for countcount in sis.inter_cout.values():
                self.db.saveCountData(countcount, sis.begin_time, sis.end_time)
            for elapsedcount in sis.inter_time.values():
                self.db.saveElapsedData(elapsedcount, sis.begin_time, sis.end_time)
            for currcount in sis.curr_task_num:
                self.db.saveCurrTaskData(currcount, sis.end_time)
            self.db.saveStatusData(hostname, sis.fixed_info, sis.dyna_info, sis.end_time)
        self.db.clearData()

class CSyncTimer():
    def __init__(self):
        self.sis = StatisticsTask()
        self.timer = TimerHandle(10, self.sis.syncStatisticsToDb)

    def start(self):
        self.timer.start()

    def cancel(self):
        self.timer.cancel()

    