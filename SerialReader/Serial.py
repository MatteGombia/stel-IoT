import serial
import serial.tools.list_ports

class SerialReader:
    def __init__(self, id):
        self.setupSerial()
        self.id = id
    
    def setupSerial(self):
		# open serial port
        self.ser = None

        print("list of available ports: ")
        ports = serial.tools.list_ports.comports()  

        for port in ports:
            print (port.device)
            print (port.description)
            if "arduino" in port.description.lower():
                self.portname = port.device      
        try:
            if self.portname is not None:
                print ("connecting to " + self.portname)
                self.ser = serial.Serial(self.portname, 9600, timeout=0)
        except:
            self.ser = None

		# self.ser.open()

		# internal input buffer from serial
        self.inbuffer = []

    def loop(self, thingsBoardClient):
		# infinite loop for serial managing
		#
        while (True):
			#look for a byte from serial
            if not self.ser is None:

                if self.ser.in_waiting>0:
					# data available from the serial port
                    lastchar=self.ser.read(1)

                    if lastchar==b'}':
                        print("\nValue received")
                        thingsBoardClient.send_telemetry(self.id, str(self.inbuffer))
                        self.inbuffer = []
                    else:
                        # append
                        self.inbuffer.append (lastchar)

if __name__ == '__main__':
	sr=SerialReader()