import socket

class Socket:
    def __init__(self):
        self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serversocket.bind(('localhost', 8089))
        self.serversocket.listen(5)
        self.id = {
            "192.168.1.1": "1.1",
            "127.0.0.1": "1.2",
            }

    def read(self, thingsBoardClient):
        while True:
            connection, address = self.serversocket.accept()
            print(address[0])
            while True:
                buf = connection.recv(64)
                if len(buf) > 0:
                    print(str(buf))
                    #thingsBoardClient.send_telemetry(self.id[address[0]], buf)
                    connection.close()
                    break
if __name__ == '__main__':
    soc = Socket(1)
    while True:
      soc.read()