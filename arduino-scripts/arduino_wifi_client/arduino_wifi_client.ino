#include "WiFiManager.h"
#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include "arduino_secrets.h"
#include <ArduinoSTL.h>
#include <map>

#define RS_PIN (12)
#define EN_PIN (11)
#define D7_PIN (2)
#define D6_PIN (3)
#define D5_PIN (4)
#define D4_PIN (5)
#define POT_PIN A3

#define GET_REQUEST_INTERVAL (500)

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char serverAddress[] = "192.168.0.100"; 
int port = 8080;

long lastGet = 0;

WiFiManager wifiManager(SECRET_SSID, SECRET_PASS, serverAddress, port);

LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

std::map<String, String> ids = { //dict for stalls and bathrooms ids
    {"1.1", "2f79fd90-b162-11ef-aaf6-af873acb8c03"},
    {"1.2", "0501d6e0-b163-11ef-aaf6-af873acb8c03"},
    {"2.1", "4e045f10-b164-11ef-aaf6-af873acb8c03"},
    {"2.2", "58f79040-b164-11ef-aaf6-af873acb8c03"},
    {"1", "c3ae1950-e561-11ef-a4ea-33e3923212f2"},
    {"2", "e71e8460-e561-11ef-a4ea-33e3923212f2"}
};

std::map<String, String> responses; //dict for the http responses 

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  wifiManager.connect();
}

void loop() {
  if(millis() - lastGet >= GET_REQUEST_INTERVAL){
    for (const auto& pair : ids) {
      String endpoint = "/api/plugins/telemetry/DEVICE/" + pair.second + "/values/timeseries";
      String authHeader = "Bearer eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ0ZW5hbnRAdGhpbmdzYm9hcmQub3JnIiwidXNlcklkIjoiMmI2YWUyMjAtYjE1Yi0xMWVmLWFkM2YtODE1ZWRhNzJiNTg0Iiwic2NvcGVzIjpbIlRFTkFOVF9BRE1JTiJdLCJzZXNzaW9uSWQiOiI0MWM3MzAwNS04ZjRhLTQ2YmEtOWM0MS1hOWQ0ZjhlYTNlYWEiLCJleHAiOjE3NDIwMzgxODksImlzcyI6InRoaW5nc2JvYXJkLmlvIiwiaWF0IjoxNzM5MDM4MTkwLCJlbmFibGVkIjp0cnVlLCJpc1B1YmxpYyI6ZmFsc2UsInRlbmFudElkIjoiMmIzZjE3MzAtYjE1Yi0xMWVmLWFkM2YtODE1ZWRhNzJiNTg0IiwiY3VzdG9tZXJJZCI6IjEzODE0MDAwLTFkZDItMTFiMi04MDgwLTgwODA4MDgwODA4MCJ9.oWZccfVAiA-54XDCi4Pevd0roJZeEmjKpy3LqAscxPr2VVcdrxaO2gELQH5o9lQeZ9SVg7pyfr7O4Wz--ZCK8w";

      responses[pair.first] = wifiManager.sendGetRequest(endpoint.c_str(), authHeader.c_str());
    }
    for (const auto& pair : responses) {
      Serial.print("Key: ");
      Serial.print(pair.first);    
      Serial.print(", Value: ");
      Serial.println(pair.second); 
    }
    lastGet = millis();
    JsonDocument bath_1;
    JsonDocument bath_2;
    const char* people_1;
    const char* people_2;
    DeserializationError error = deserializeJson(bath_1, responses["1"]);

    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
    } else {
      JsonArray people = bath_1["People"].as<JsonArray>();
      people_1 = people[0]["value"];
    }

    error = deserializeJson(bath_2, responses["2"]);

    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
    } else {
      JsonArray people = bath_2["People"].as<JsonArray>();
      people_2 = people[0]["value"];
    }

    //reading from the potentiometer pin
    int potVal = analogRead(POT_PIN);
    printf(potVal);
    Serial.println(potVal);
    lcd.clear();
    if(potVal < 256){
      Serial.println("First Bathroom");
      Serial.println("First Stall");

      lcd.setCursor(0, 0);
      lcd.print("1B people:");
      lcd.setCursor(10, 0);
      lcd.print(people_1);
      lcd.setCursor(0,1);
      lcd.print("1S");
    }
    else if(potVal < 512){
      Serial.println("First Bathroom");
      Serial.println("Second Stall");

      lcd.setCursor(0, 0);
      lcd.print("1B people:");
      lcd.setCursor(10, 0);
      lcd.print(people_1);
      //lcd.setCursor(0, 0);
      //lcd.print("First Bathroom");
      lcd.setCursor(0,1);
      lcd.print("2S");
    }
    else if(potVal < 768){
      Serial.println("Second Bathroom");
      Serial.println("First Stall");

      lcd.setCursor(0, 0);
      lcd.print("2B people:");
      lcd.setCursor(10, 0);
      lcd.print(people_2);
      //lcd.setCursor(0, 0);
      //lcd.print("Second Bathroom");
      lcd.setCursor(0,1);
      lcd.print("1S");
    }
    else{
      Serial.println("Second Bathroom");
      Serial.println("Second Stall");

      lcd.setCursor(0, 0);
      lcd.print("2B people:");
      lcd.setCursor(10, 0);
      lcd.print(people_2);
      //lcd.setCursor(0, 0);
      //lcd.print("Second Bathroom");
      lcd.setCursor(0,1);
      lcd.print("2S");
    }
  }
}
