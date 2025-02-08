import requests

import configparser


class ThingsBoardClient:
    def __init__(self):
        """
        Initializes the ThingsBoardClient.
        """
        self.access_token = {
            "1.1": "i60Zg6uPxUvxFeiaPOy3",
            "1.2": "b99WddfhzlrJ6QelBFf0",
            "2.1": "8io8PLY1T99qMP6eTP14",
            "2.2": "2ynVlOLlTpxXpKUVRao5",
            "1": "Z6P7VFvB5op6FvWhgFdS",
            "2": "LX8h6bpQrgJskyFtcsv5"
            }

        self.config = configparser.ConfigParser()
        self.config.read('config.ini')
        self.base_url = self.config.get("Http","Server", fallback= "http://192.168.1.49:8080")
    
    def send_telemetry(self, device, data):
        """
        Sends telemetry data to ThingsBoard.

        :param device: Number of the bathroom
        :param data: String in Json
        :return: The response from the ThingsBoard server
        """
        url = f"{self.base_url}/api/v1/{self.access_token[device]}/telemetry"
        headers = {
            "Content-Type": "application/json"
        }
        try:
            response = requests.post(url, data=data, headers=headers)
            response.raise_for_status()
            return response.json() if response.content else {"status": "success"}
        except requests.exceptions.RequestException as e:
            print(f"Error sending telemetry: {e}")
            return {"status": "error", "message": str(e)}

# Example usage:
if __name__ == "__main__":
    # Initialize the client
    client = ThingsBoardClient()
    
    # Telemetry data to send
    telemetry = "{'Occupied': False}"
    
    # Send telemetry
    response = client.send_telemetry("1.1", telemetry)
    print(response)
