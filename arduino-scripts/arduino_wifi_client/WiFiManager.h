#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

class WiFiManager {
  public: 
    WiFiManager(const char* ssid, const char* password, const char* serverAddress, int port);
    void connect();
    String sendGetRequest(const char* endpoint, const char* authHeader);

  private:
    char _ssid[32];
    char _password[64];
    char _serverAddress[64];
    int _port;
    WiFiClient _wifi;
    HttpClient _client;
};

#endif