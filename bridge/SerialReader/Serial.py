import time
import serial
import serial.tools.list_ports
import json

class SerialReader:
    def __init__(self, id, port):
        self.portname = port
        self.setupSerial()
        self.id = id
    
    def setupSerial(self):
        print()
		# open serial port
        self.ser = None

        print("list of available ports: ")
        ports = serial.tools.list_ports.comports()  
        print("portname: " + self.portname)    
        for port in ports:
            print (port.device)
            print (port.description)
#            if "usb2.0-serial" in port.description.lower():
#                self.portname = port.device
        try:
            if self.portname is not None:
                print ("connecting to " + self.portname)
                self.ser = serial.Serial(self.portname, 9600, timeout=0)
                print()    
                print()    
        except:
            self.ser = None
            print("Not Connecting")
		# self.ser.open()

		# internal input buffer from serial
        self.inbuffer = []

    def loop(self, thingsBoardClient, sup, stall):
		# infinite loop for serial managing
		#
        while (True):
			#look for a byte from serial
            if not self.ser is None:

                if self.ser.in_waiting>0:
					# data available from the serial port
                    lastchar=self.ser.read(1)

                    self.inbuffer.append(lastchar)

                    if lastchar==b'}':
                        print("\nValue received")
                        #print("Complete buffer: ", str(self.inbuffer))

                        #merge the bytes into a string
                        data_str = b''.join(self.inbuffer).decode('utf-8')

                        try:
                            #try deserializing as json
                            data_dict = json.loads(data_str)
                            print("Data received as JSON:", data_dict)

                            sup.stateStall[stall-1] = data_dict["State"]
                            #send data
                            thingsBoardClient.send_telemetry(self.id, json.dumps(data_dict))

                        except json.JSONDecodeError as e:
                            print(f"Error while decoding the JSON: {e}")

                        #reset the buffer for the next transmission
                        self.inbuffer = []

if __name__ == '__main__':
	sr=SerialReader()
