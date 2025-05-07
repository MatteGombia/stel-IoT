#include "WiFiManager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password, const char* serverAddress, int port)
  :_client(_wifi, serverAddress, port) {
    strncpy(_ssid, ssid, sizeof(_ssid));
    strncpy(_password, password, sizeof(_password));
    strncpy(_serverAddress, serverAddress, sizeof(_serverAddress));
    _port = port;
}

void WiFiManager::connect() {
    int status = WL_IDLE_STATUS;
    
    Serial.print("Attempting to connect to Network: ");
    Serial.println(_ssid);

    while (status != WL_CONNECTED) {
        status = WiFi.begin(_ssid, _password);
    }

    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

String WiFiManager::sendGetRequest(const char* endpoint, const char* authHeader) {
    Serial.println("Making GET request...");
    _client.beginRequest();
    _client.get(endpoint);
    _client.sendHeader("X-Authorization", authHeader);
    _client.endRequest();

    int statusCode = _client.responseStatusCode();
    String response = _client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);

    return response;
}

