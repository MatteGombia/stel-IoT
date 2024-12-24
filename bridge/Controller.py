from NetworkReader.Socket import *
from HttpApi.ThingsBoardClient import *
from SerialReader.Serial import *

import threading

class Controller:
    def __init__(self):
        self.socket = Socket()
        self.serial = SerialReader(5)
        self.thingsBoardClient = ThingsBoardClient()

        t1 = threading.Thread(target=self.socket.read, args=(self.thingsBoardClient, ))
        t2 = threading.Thread(target=self.serial.loop, args=(self.thingsBoardClient, ))

        t1.start()
        t2.start()

        
if __name__ == '__main__':
    con = Controller()