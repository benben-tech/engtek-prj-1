#define DEBUG

#ifdef DEBUG
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
  pinMode(LED_BUILTIN, OUTPUT);
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  DEBUG_PRINT("Start blinking");
}

void loop() {
  DEBUG_PRINT("HIGH");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);

  DEBUG_PRINT("LOW");
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
