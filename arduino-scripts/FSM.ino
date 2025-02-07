// FSM 
//
//left_button=WARNING
//center_button=CLEANING
//right_button=MAINTENANCE

#include <ArduinoJson.h>

//States:
enum State {Occupied, Free, Unusable, Maintenance, Cleaning};
//inputs:
enum Input {DETECTED, NOT_DETECTED, WARNING, MAINTENANCE, CLEANING, OK};
// outputs:
enum Output {RED_ON, GREEN_ON, YELLOW_ON};

State iCurrentState;

unsigned long lastSend = 0;
unsigned long lastPerception = 0;
float distance = 0;

#define TRIG_PIN (2)
#define ECHO_PIN (4)
#define WRN_PIN (3)
#define MAINT_PIN (5)
#define CLEAN_PIN (6)
#define OK_PIN (7)
#define RED_PIN (10)
#define GREEN_PIN (9)
#define YELLOW_PIN (11)
#define THRESHOLD_DIST (50)
#define PERCEPTION_INTERVAL (100)
#define SEND_INTERVAL (1000)

int detection()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  unsigned long timing = pulseIn(ECHO_PIN, HIGH);
    if (timing == 0) {
    	return NOT_DETECTED;  // No echo received
    }
    distance = (timing * 0.034) / 2;
  	if(distance >= 0 && distance <= THRESHOLD_DIST){
    	return DETECTED;
    }
    else{
      return NOT_DETECTED;
    }
}

void sendJSON(State state, float distance);

void setup()
{
  iCurrentState = Free; //initial state
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(WRN_PIN, INPUT);
  pinMode(MAINT_PIN, INPUT);
  pinMode(CLEAN_PIN, INPUT);
  pinMode(OK_PIN, INPUT);
  
  digitalWrite(TRIG_PIN, LOW);
  Serial.begin(9600);
}

void loop()
{
  if((millis() - lastPerception) >= PERCEPTION_INTERVAL){
    // read the input and convert it into the input symbol
    Input iInput = (Input)detection();
    lastPerception = millis();
    
    if((digitalRead(WRN_PIN) == HIGH)){
      iInput = WARNING;
    }
    //if((digitalRead(MAINT_PIN) == HIGH)){
      //iInput = MAINTENANCE;
    //}
    //if((digitalRead(CLEAN_PIN) == HIGH)){
      //iInput = CLEANING;
    //}
    if((digitalRead(OK_PIN) == HIGH)){
      iInput = OK;
    }

    //Function F (future state)
    State iFutureState;
    iFutureState = iCurrentState; // default: same state
    
    switch (iCurrentState){
      case Occupied:
        if(iInput == NOT_DETECTED){
            iFutureState = Free;
        }
        else if(iInput == WARNING){
            iFutureState = Unusable;
        }
        break;
    case Free:
        switch (iInput){
          case DETECTED:
            iFutureState = Occupied;
            break;
          case MAINTENANCE:
            iFutureState = Maintenance;
            break;
          case CLEANING:
            iFutureState = Cleaning;
            break;
          case WARNING:
            iFutureState = Unusable;
            break;
          }
        break;
      case Unusable:
        switch (iInput){
          case MAINTENANCE:
            iFutureState = Maintenance;
            break;
          case CLEANING:
            iFutureState = Cleaning;
            break;
          case OK:
            iFutureState = Free;
            break;
          }
        break;
      case Maintenance:
        if(iInput == OK){
            iFutureState = Free;
        }
        break;
      case Cleaning:
        if(iInput == OK){
            iFutureState = Free;
        }
        break;
    }

      
    // state transition
    iCurrentState= iFutureState;
    
      
    //Function G
    Output output;
    
    switch (iCurrentState){
      case Occupied:
      case Maintenance:
      case Cleaning:
        output = RED_ON;
        break;
      
      case Free:
        output = GREEN_ON;
        break;
      
      case Unusable:
        output = YELLOW_ON;
        break;
    }
    
    //  output symbols -> actuators
    switch (output){
      case RED_ON:
        digitalWrite(RED_PIN , HIGH);  
        digitalWrite(GREEN_PIN , LOW);
        digitalWrite(YELLOW_PIN , LOW);
        break;
      
      case GREEN_ON:
        digitalWrite(GREEN_PIN , HIGH); 
        digitalWrite(RED_PIN , LOW);
        digitalWrite(YELLOW_PIN , LOW);
        break;

      case YELLOW_ON:
        digitalWrite(YELLOW_PIN , HIGH); 
        digitalWrite(RED_PIN , LOW);
        digitalWrite(GREEN_PIN , LOW);
        break;
    }
  }
    // sending the current state via serial
    
  if((millis()- lastSend) >= SEND_INTERVAL){
    sendJSON(iCurrentState, distance);
    lastSend = millis();
  }
}

void sendJSON(State state, float distance) {
  // sends the current state in JSON format
  String stateStr = "";

  switch (state) {
    case Occupied:
      stateStr = "Occupied";
      break;
    case Free:
      stateStr = "Free";
      break;
    case Unusable:
      stateStr = "Unusable";
      break;
    case Maintenance:
      stateStr = "Maintenance";
      break;
    case Cleaning:
      stateStr = "Cleaning";
      break;
  }

  // sends the JSON state
  JsonDocument doc;
  doc["State"] = stateStr;
  doc["Distance"] = distance;
  serializeJson(doc, Serial);
  Serial.println("");
  //Serial.print("{\"State\": \"");
  //Serial.print(stateStr);
  //Serial.print("\", \"Distance\": \"");
  //Serial.print(String(distance));
  //Serial.println("\"}");
}