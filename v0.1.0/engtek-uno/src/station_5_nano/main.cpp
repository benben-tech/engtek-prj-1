#include <Arduino.h>
#include <SensorLimit.h>
// #include <ControlButton.h>
const int8_t PIN_CONTROL_BUTTON=A0;
const int8_t PIN_ANALOG=A0;
const int8_t PIN_PROXIMITY= 5;
int _id = 0;
int _data=0;

SensorLimit sensorLimit(PIN_CONTROL_BUTTON);
// ControlButton controlButton(PIN_PROXIMITY);

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
  Serial.begin(9600);
  DEBUG_PRINT("Start");
    /* Set up Sensor Limit callbacks */
  sensorLimit.setOnClickCallback(onClick);
  sensorLimit.setOnShortPressCallback(onShortPress);
  sensorLimit.setOnLongPressCallback(onLongPress);

//   controlButton.setOnClickCallback(onClick);
//   controlButton.setOnShortPressCallback(onShortPress);
//   controlButton.setOnLongPressCallback(onLongPress);

  /* Initialize Sensor Limit */
  sensorLimit.begin();

//   controlButton.begin();
}

void loop() {
  sensorLimit.handle();
//   controlButton.handle();
  _data = map(analogRead(PIN_ANALOG),0,1023,0,1023);
  DEBUG_PRINT(_data);
  // Serial.println(digitalRead(PIN_ANALOG));
}

void onClick(void) {
  _id++;
  DEBUG_PRINT(_id);
}

void onShortPress(void) {
  DEBUG_PRINT("DETECT");
}

void onLongPress(void) {
  DEBUG_PRINT("SEND DATA");
  nodeMCUsend(_data,_id);
}

void nodeMCUsend(int _data, int _id){
  DEBUG_PRINT((String)_id+","+(String)_data);
  Serial.println((String)_id+","+(String)_data);
}