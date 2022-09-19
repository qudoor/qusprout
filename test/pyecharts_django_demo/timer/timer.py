from threading import Thread
from threading import Event

class TimerHandle(Thread):
    def __init__(self, interval, function):
        Thread.__init__(self)
        self.interval = interval
        self.function = function
        self.finished = Event()

    def cancel(self):
        self.finished.set()

    def run(self):
        while True:
            print("----TimerHandle run------")
            self.finished.wait(self.interval)
            if not self.finished.isSet():
                self.function()
            else:
                break
