import uuid

from thrift.transport import TSocket

from thrift.transport import TTransport

from thrift.protocol import TBinaryProtocol, TMultiplexedProtocol

from qusprout.master import MasterServer

import qusprout.master.ttypes



class ResourceServer:

    def __init__(self, ip='localhost', port=9092):

        socket = TSocket.TSocket(ip, port)

        self.transport = TTransport.TBufferedTransport(socket)

        protocol = TBinaryProtocol.TBinaryProtocol(self.transport)

        quest = TMultiplexedProtocol.TMultiplexedProtocol(protocol, "MasterServer")

        self.client = MasterServer.Client(quest)

        self.transport.open()



    def close(self):

        self._transport.close()



    def gettatistics(self):

        req = qusprout.master.ttypes.GetStatisticsInfoReq(uuid.uuid4().hex)

        resp = self.client.GetStatisticsInfo(req)

        return resp

