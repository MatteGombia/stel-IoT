import time


class SupervisorClass():
    def __init__(self, id):
        self.stateStall = ["Free", "Free"]
        self.id = id

    def loop(self, thingsBoardClient):
        while(True):
            print("State stalls: [" + self.stateStall[0] + ", " + self.stateStall[1] + "]")
            
            if self.stateStall[0] == "Unusable" and self.stateStall[1] == self.stateStall[0]:
                state = "Unavailable"
            elif self.stateStall[0] != "Free" and self.stateStall[1] != "Free":
                state = "Full"
            else:
                state = "Available"
            
            print(state)
            thingsBoardClient.send_telemetry(self.id, "{State: " + state + "}")
            time.sleep(1)