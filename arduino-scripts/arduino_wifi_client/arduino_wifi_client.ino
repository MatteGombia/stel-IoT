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
#define DISPLAY_INTERVAL (100) //not used

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char serverAddress[] = "192.168.0.100"; 
int port = 8080;

long lastGet = 0;
long lastDisplay = 0; //not used

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
std::map<String, String> states = { //dict for the stalls state
    {"1.1", ""},
    {"1.2", ""},
    {"2.1", ""},
    {"2.2", ""}
};
std::map<String, String> toilet_papers = { //dict for the stalls toilet paper percentage 
    {"1.1", ""},
    {"1.2", ""},
    {"2.1", ""},
    {"2.2", ""}
};
std::map<String, String> num_people = { //dict for the number of people in queue for each bathroom
    {"1", ""},
    {"2", ""}
};

void display_info(int potVal, std::map<String, String> num_people, std::map<String, String> states, std::map<String, String> toilet_papers);

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

    //deserializing the number of people in each bathroom from the JSON
    for (const auto& pair : num_people){
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, responses[pair.first]);
      if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
      } else {
        JsonArray people = doc["People"].as<JsonArray>();
        num_people[pair.first] = people[0]["value"];
      }
    }

    //deserializing the State and the Toilet Paper from the JSON
    for (const auto& pair : states){
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, responses[pair.first]);
      if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
      } else {
        JsonArray state = doc["State"].as<JsonArray>();
        states[pair.first] = state[0]["value"];
        JsonArray toilet_paper = doc["ToiletPaper"].as<JsonArray>();
        toilet_papers[pair.first] = toilet_paper[0]["value"];
      }
    }

    //reading from the potentiometer pin
    int potVal = analogRead(POT_PIN);
    printf(potVal);
    Serial.println(potVal);
    
    //display bathrooms/stalls info based on potVal
    display_info(potVal, num_people, states, toilet_papers);
    lastDisplay = millis();
  }
}

void display_info(int potVal, std::map<String, String> num_people, std::map<String, String> states, std::map<String, String> toilet_papers){
  lcd.clear();

  String state;
  String people_text;
  String tp_text;
  String stall_label;

  if(potVal < 256){
    people_text = "1B people:" + num_people["1"];
    stall_label = "1S ";
    tp_text = " tp:" + toilet_papers["1.1"] + "%";
    state = states["1.1"].substring(0, min(13 - tp_text.length(), states["1.1"].length()));
  } 
  else if(potVal < 512){
    people_text = "1B people:" + num_people["1"];
    stall_label = "2S ";
    tp_text = " tp:" + toilet_papers["1.2"] + "%";
    state = states["1.2"].substring(0, min(13 - tp_text.length(), states["1.2"].length()));
  }
  else if(potVal < 768){
    people_text = "2B people:" + num_people["2"];
    stall_label = "1S ";
    tp_text = " tp:" + toilet_papers["2.1"] + "%";
    state = states["2.1"].substring(0, min(13 - tp_text.length(), states["2.1"].length()));
  }
  else{
    people_text = "2B people:" + num_people["2"];
    stall_label = "2S ";
    tp_text = " tp:" + toilet_papers["2.2"] + "%";
    state = states["2.2"].substring(0, min(13 - tp_text.length(), states["2.2"].length()));
  }

  lcd.setCursor(0, 0);
  lcd.print(people_text);

  lcd.setCursor(0, 1);
  lcd.print(stall_label);
  lcd.print(state);

  lcd.setCursor(16 - tp_text.length(), 1);
  lcd.print(tp_text);

}
