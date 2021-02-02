#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <ControlButton.h>
// #include <SoftwareSerial.h>

// SoftwareSerial nodeMCU(2,3);

const int8_t PIN_CONTROL_BUTTON=2;
const int8_t PIN_LED=13;

int _id = 0;
bool _ledState = HIGH; 

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
#endif

ControlButton controlButton(PIN_CONTROL_BUTTON);

void onClick(void);
void onShortPress(void);
void onLongPress(void);
void nodeMCUsend(int _data, int _id);

void setup() {
  Serial.begin(115200);
  // nodeMCU.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, !_ledState);
  DEBUG_PRINT("Start");
    /* Set up Control Button callbacks */
  controlButton.setOnClickCallback(onClick);
  controlButton.setOnShortPressCallback(onShortPress);
  controlButton.setOnLongPressCallback(onLongPress);

  /* Initialize Control Button */
  controlButton.begin();
}

void loop() {
  controlButton.handle();
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
  nodeMCUsend(random(1024),_id);
}

void nodeMCUsend(int _data, int _id){
  DEBUG_PRINT((String)_id+","+(String)_data);
  digitalWrite(PIN_LED,!_ledState);
  // nodeMCU.println((String)_id+","+(String)_data);
}
