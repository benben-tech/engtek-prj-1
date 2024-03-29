#include <Arduino.h>
// #include <Arduino_FreeRTOS.h>

// #include <SoftwareSerial.h>
// SoftwareSerial nodeMCU(2,3);

#include <SensorLimit.h>
#include <ControlButton.h>
const int8_t PIN_CONTROL_BUTTON=A5;
const int8_t PIN_LED=13;
const int8_t PIN_ANALOG=A5;
const int8_t PIN_PROXIMITY=5;
int _id = 0;
bool _ledState = HIGH; 
int _data=0;

SensorLimit sensorLimit(PIN_CONTROL_BUTTON);
ControlButton controlButton(PIN_PROXIMITY);

void onClick(void);
void onShortPress(void);
void onLongPress(void);
void nodeMCUsend(int _data, int _id);


#if DEBUG == 1
  #define DEBUG_PRINT(x) \
  Serial.print(millis()); \
  Serial.print(": "); \
  Serial.print(__PRETTY_FUNCTION__); \
  Serial.print(" in "); \
  Serial.print(__FILE__); \
  Serial.print(":"); \
  Serial.print(__LINE__); \
  Serial.print(" "); \
  Serial.println(x);
#else
  #define DEBUG_PRINT(x)
#endif



void setup() {

  Serial.begin(115200);
  // nodeMCU.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, !_ledState);
  DEBUG_PRINT("Start");
    /* Set up Sensor Limit callbacks */
  sensorLimit.setOnClickCallback(onClick);
  sensorLimit.setOnShortPressCallback(onShortPress);
  sensorLimit.setOnLongPressCallback(onLongPress);

  controlButton.setOnClickCallback(onClick);
  controlButton.setOnShortPressCallback(onShortPress);
  controlButton.setOnLongPressCallback(onLongPress);

  /* Initialize Sensor Limit */
  sensorLimit.begin();

  controlButton.begin();

}

void loop() {
  sensorLimit.handle();
  controlButton.handle();
  _data = map(analogRead(PIN_ANALOG),0,1023,0,1023);
  // DEBUG_PRINT(data);

}


void onClick(void) {
  _id++;
  DEBUG_PRINT(_id);
  digitalWrite(PIN_LED,_ledState);
}

void onShortPress(void) {
  DEBUG_PRINT("DETECT");
}

void onLongPress(void) {
  DEBUG_PRINT("SEND DATA");
  nodeMCUsend(_data,_id);
}

void nodeMCUsend(int _data, int _id){
  // DEBUG_PRINT((String)_id+","+(String)_data);
  digitalWrite(PIN_LED,!_ledState);
  Serial.println((String)_id+","+(String)_data);
}