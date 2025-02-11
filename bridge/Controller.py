from NetworkReader.Socket import *
from HttpApi.ThingsBoardClient import *
from SerialReader.Serial import *
from Yolo.YOLO_Detector import *
from Supervisor.SupervisorClass import *

import threading



class Controller:
    def __init__(self):
        #self.socket = Socket()
        self.serial1 = SerialReader("2.1", "/dev/ttyUSB0")
        self.serial2 = SerialReader("2.2", "/dev/ttyUSB1")
        self.model = YOLO_Detector("2")
        self.thingsBoardClient = ThingsBoardClient()
        self.sup = SupervisorClass("2")

        t0 = threading.Thread(target=self.sup.loop, args=(self.thingsBoardClient, )) 
        #t1 = threading.Thread(target=self.socket.read, args=(self.thingsBoardClient, ))
        t2 = threading.Thread(target=self.serial1.loop, args=(self.thingsBoardClient, self.sup, 1))
        t3 = threading.Thread(target=self.serial2.loop, args=(self.thingsBoardClient, self.sup, 2))
        t4 = threading.Thread(target=self.model.loop, args=(self.thingsBoardClient, )) 
 
        t0.start()
        #t1.start()
        t2.start()
        t3.start()
        t4.start()
        
if __name__ == '__main__':
    con = Controller()
