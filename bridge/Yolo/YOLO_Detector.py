import configparser
from ultralytics import YOLO
import cv2
import time

class YOLO_Detector():

    def __init__(self, id):
        self.id = id
        self.config = configparser.ConfigParser()
        self.config.read('config.ini')

        self.show = self.config.getboolean("Detector", "ShowVideo", fallback=True)
        self.camera = self.config.get("DEFAULT", "Camera", fallback="/dev/video0")
        print(self.camera)

        self.setupDetector()

    def setupDetector(self):
        # Start webcam
        self.cap = cv2.VideoCapture(self.camera)
        if not self.cap.isOpened():
            print("Error: Camera not found")
            exit(1)

        self.cap.set(3, 640)
        self.cap.set(4, 480)

        # Load model
        self.model = YOLO("best.pt", verbose=True)

    def loop(self, thingsBoardClient):
        iteration = 0
        while iteration < 1000:  # Prevent infinite loops
            success, img = self.cap.read()
            if not success:
                print("Error: Failed to capture image")
                break  # Exit the loop if the camera feed fails

            results = self.model.predict(img, verbose=True)

            if self.show:
                annotated_frame = results[0].plot()
                cv2.imshow("YOLO Inference", annotated_frame)

            if cv2.waitKey(1) & 0xFF == ord("q"):
                break

            iteration += 1  # Prevent indefinite looping
            if iteration % 5 == 0:
                self.cap.release()
                time.sleep(1)  # Let the camera reset
                self.cap = cv2.VideoCapture(self.camera)
            else:
                time.sleep(1)
                count_obj = 0
                for box in results[0].boxes:
                    count_obj += 1

                print(count_obj)
                    
                thingsBoardClient.send_telemetry(self.id, "{People: " + str(count_obj) + "}")

        self.cap.release()
        cv2.destroyAllWindows()

if __name__ == '__main__':
    dt = YOLO_Detector("1")
    dt.loop(None)
