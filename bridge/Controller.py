from NetworkReader.Socket import *
from HttpApi.ThingsBoardClient import *
from SerialReader.Serial import *
from Yolo.YOLO_Detector import *

import threading

class Controller:
    def __init__(self):
        #self.socket = Socket()
        #self.serial1 = SerialReader("1.1", "/dev/ttyUSB0")
        #self.serial2 = SerialReader("1.2", "/dev/ttyUSB1")
        self.model = YOLO_Detector("2")
        self.thingsBoardClient = ThingsBoardClient()

        #t1 = threading.Thread(target=self.socket.read, args=(self.thingsBoardClient, ))
        #t2 = threading.Thread(target=self.serial1.loop, args=(self.thingsBoardClient, ))
        #t3 = threading.Thread(target=self.serial2.loop, args=(self.thingsBoardClient, ))
        t4 = threading.Thread(target=self.model.loop, args=(self.thingsBoardClient, )) 
 
        #t1.start()
        #t2.start()
        #t3.start()
        t4.start()

        
if __name__ == '__main__':
    con = Controller()
