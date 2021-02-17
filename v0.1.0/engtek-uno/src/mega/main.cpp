#include <Arduino.h>

const int8_t PIN_LED=13;
bool _ledState = HIGH; 
int _id = 0;

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

void nodeMCUsend(int _id, String _data1,String _data2,String _data3);


void readSerialFromCamera(void);
String readSerial();
void Processbuffer(String buffer);

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600); //camera
  Serial3.begin(9600); //nodemcu
  // Serial3.println("TEST");
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, !_ledState);
  DEBUG_PRINT("Start");

}

void loop() {
  readSerialFromCamera();
}

void readSerialFromCamera(){
  String buffer = readSerial();
  if (buffer.endsWith("\n")){
      buffer.trim();
      Processbuffer(buffer);
  }
}

String readSerial()
{
    String buffer;
    while (Serial2.available())
    {
        char c = Serial2.read();
        buffer.concat(c);
        delay(10);
    }
    Serial2.flush();
    return buffer;
}

void Processbuffer(String buffer) {
  int ind1, ind2, ind3, ind4;
  String data1, data2, data3, data4;
  _id++;
  //buffer.toLowerCase();
  DEBUG_PRINT("Data from Camera: " + (String) buffer);
  ind1 = buffer.indexOf(',');  //finds location of first ','
  ind2 = buffer.indexOf(',', ind1 + 1);
  ind3 = buffer.indexOf(',', ind2 + 1);
  ind4 = buffer.indexOf(',', ind3 + 1);
  
  data1 = buffer.substring(0, ind1);  
  data2 = buffer.substring(ind1 + 1, ind2); 
  data3 = buffer.substring(ind2 + 1, ind3); 
  data4 = buffer.substring(ind3 + 1, ind4); 
  // buffer.substring(ind1 + 1, buffer.length()); 
  // postData(data1,data2);
  data1.replace(" ", "");
  data2.replace(" ", "");
  data3.replace(" ", "");
  nodeMCUsend(_id,data1,data2,data3);
  // Serial.println("1st data: " + data1);
  // Serial.println("1st data: " + data2);
  // Serial.println("1st data: " + data3);
  buffer = "";
}

void nodeMCUsend(int _id, String _data1,String _data2,String _data3){
  // DEBUG_PRINT((String)_id+","+(String)_data);
  digitalWrite(PIN_LED,!_ledState);
  Serial3.println((String)_id+","+(String)_data1+","+(String)_data2+","+(String)_data3);
}