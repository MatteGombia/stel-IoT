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

char serverAddress[] = "192.168.1.58"; //TO BE CHECKED THE DAY OF THE EXAM
int port = 8080;

long lastGet = 0;
long lastDisplay = 0; //not used

WiFiManager wifiManager(SECRET_SSID, SECRET_PASS, serverAddress, port);

LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

std::map<String, String> ids = { //dict for stalls and bathrooms ids
    {"1.1", "a29906c0-e7fb-11ef-8c48-2795bb74502b"},//"2f79fd90-b162-11ef-aaf6-af873acb8c03"},
    {"1.2", "a2802790-e7fb-11ef-8c48-2795bb74502b"},//"0501d6e0-b163-11ef-aaf6-af873acb8c03"}, 
    {"2.1", "a2a19240-e7fb-11ef-8c48-2795bb74502b"},//"4e045f10-b164-11ef-aaf6-af873acb8c03"},
    {"2.2", "a2abcb70-e7fb-11ef-8c48-2795bb74502b"},//"58f79040-b164-11ef-aaf6-af873acb8c03"},
    {"1", "a2b89cb0-e7fb-11ef-8c48-2795bb74502b"},//"c3ae1950-e561-11ef-a4ea-33e3923212f2"},
    {"2", "a2be9020-e7fb-11ef-8c48-2795bb74502b"}//"e71e8460-e561-11ef-a4ea-33e3923212f2"}
};

String stall_response; //http response for stall
String bathroom_response; //http response for bathroom
String stall_state;
String toilet_paper;
String num_people;
String bathroom_state;
JsonDocument doc;
String endpoint;
String authHeader = "Bearer eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ0ZW5hbnRAdGhpbmdzYm9hcmQub3JnIiwidXNlcklkIjoiMDQ2YTU2MjAtZTdmYi0xMWVmLWJjMzQtZDE4MThhMzQxZjNiIiwic2NvcGVzIjpbIlRFTkFOVF9BRE1JTiJdLCJzZXNzaW9uSWQiOiI3MjhmNWNhMS1iZWRjLTQ4ZjUtYjJiYy0wMDhlNGNmMGI5NTMiLCJleHAiOjE3NDAyMzg3NjMsImlzcyI6InRoaW5nc2JvYXJkLmlvIiwiaWF0IjoxNzM5Mzc0NzYzLCJlbmFibGVkIjp0cnVlLCJpc1B1YmxpYyI6ZmFsc2UsInRlbmFudElkIjoiMDNlN2RmYjAtZTdmYi0xMWVmLWJjMzQtZDE4MThhMzQxZjNiIiwiY3VzdG9tZXJJZCI6IjEzODE0MDAwLTFkZDItMTFiMi04MDgwLTgwODA4MDgwODA4MCJ9.h2bPTkUDNNFMwFAaZucREbJJlgdCAyg-5fwStRYCn2t33ehPezew8wzMS7yJfPyO2rkGiQ0Ace1fVVXmkMxz8w"; //eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ0ZW5hbnRAdGhpbmdzYm9hcmQub3JnIiwidXNlcklkIjoiMmI2YWUyMjAtYjE1Yi0xMWVmLWFkM2YtODE1ZWRhNzJiNTg0Iiwic2NvcGVzIjpbIlRFTkFOVF9BRE1JTiJdLCJzZXNzaW9uSWQiOiI0MWM3MzAwNS04ZjRhLTQ2YmEtOWM0MS1hOWQ0ZjhlYTNlYWEiLCJleHAiOjE3NDIwMzgxODksImlzcyI6InRoaW5nc2JvYXJkLmlvIiwiaWF0IjoxNzM5MDM4MTkwLCJlbmFibGVkIjp0cnVlLCJpc1B1YmxpYyI6ZmFsc2UsInRlbmFudElkIjoiMmIzZjE3MzAtYjE1Yi0xMWVmLWFkM2YtODE1ZWRhNzJiNTg0IiwiY3VzdG9tZXJJZCI6IjEzODE0MDAwLTFkZDItMTFiMi04MDgwLTgwODA4MDgwODA4MCJ9.oWZccfVAiA-54XDCi4Pevd0roJZeEmjKpy3LqAscxPr2VVcdrxaO2gELQH5o9lQeZ9SVg7pyfr7O4Wz--ZCK8w";


void display_info(String num_people, String bathroom_state, String stall_state, String toilet_paper);
void get_and_deserialize(String bath_id, String stall_id);

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  wifiManager.connect();
}

void loop() {
  if(millis() - lastGet >= GET_REQUEST_INTERVAL){

    //reading from the potentiometer pin
    int potVal = analogRead(POT_PIN);
    Serial.println(potVal);

    if(potVal < 256){
      get_and_deserialize("1", "1.1");
      display_info("1B", "1S", num_people, bathroom_state, stall_state, toilet_paper);
    } 
    else if(potVal < 512){
      get_and_deserialize("1", "1.2");
      display_info("1B", "2S", num_people, bathroom_state, stall_state, toilet_paper);
    }
    else if(potVal < 768){
      get_and_deserialize("2", "2.1");
      display_info("2B", "1S", num_people, bathroom_state, stall_state, toilet_paper);
    }
    else{
      get_and_deserialize("2", "2.2");
      display_info("2B", "2S", num_people, bathroom_state, stall_state, toilet_paper);
    }
    lastGet = millis();
  }
}

void get_and_deserialize(String bath_id, String stall_id){
  endpoint = "/api/plugins/telemetry/DEVICE/" + ids[bath_id] + "/values/timeseries";
  bathroom_response = wifiManager.sendGetRequest(endpoint.c_str(), authHeader.c_str());

  endpoint = "/api/plugins/telemetry/DEVICE/" + ids[stall_id] + "/values/timeseries";
  stall_response = wifiManager.sendGetRequest(endpoint.c_str(), authHeader.c_str());

  DeserializationError error = deserializeJson(doc, bathroom_response);
  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
  } else {
    JsonArray people = doc["People"].as<JsonArray>();
    num_people = people[0]["value"].as<String>();
    JsonArray bath_state = doc["State"].as<JsonArray>();
    bathroom_state = bath_state[0]["value"].as<String>();
  }
  //deserializing state and toilet paper
  error = deserializeJson(doc, stall_response);
  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
  } else {
    JsonArray state = doc["State"].as<JsonArray>();
    stall_state = state[0]["value"].as<String>();
    JsonArray paper = doc["ToiletPaper"].as<JsonArray>();
    toilet_paper = paper[0]["value"].as<String>();
  }
}

void display_info(String b_id, String s_id, String num_people, String bathroom_state, String stall_state, String toilet_paper){
  lcd.clear();

  lcd.setCursor(0, 0);
  String people_text = "np:" + num_people;
  String first_line = b_id + " " + bathroom_state.substring(0, min(12 - people_text.length(), bathroom_state.length())) + " " + people_text;
  lcd.print(first_line);

  lcd.setCursor(0, 1);
  String tp_text = " tp:" + toilet_paper + "%";
  String second_line = s_id + " " + stall_state.substring(0, min(13 - tp_text.length(), stall_state.length())) + tp_text;
  lcd.print(second_line);
}
