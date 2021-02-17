#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// String path = STATION;
// String ip_port = _IP_PORT;
String path = "api/station-four/";
String ip_port = "http://192.168.137.1:8000/";

const int8_t PIN_LED = 2;

const char* ssid     = "ShutdownValveAsm";  
const char* password = "engtekprecision";

void postData(String _id,String data1);
void readSerialFromUno(void);
String readSerial();
void Processbuffer(String buffer);
void connectToWifi(void);

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
  pinMode(PIN_LED, OUTPUT); 
  digitalWrite(PIN_LED, LOW);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  DEBUG_PRINT(WiFi.localIP());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(PIN_LED,LOW);
    static unsigned long wifiTimer = millis();
    const unsigned int wifiTimerInterval = 5000;
    /* Retry connection to WiFi every 5 seconds */
    if (millis() - wifiTimer >= wifiTimerInterval) {
      connectToWifi();
      wifiTimer = millis();
    }
  }else{
    digitalWrite(PIN_LED,HIGH);
  }
  readSerialFromUno();
}

void postData(String _id,String data1){
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient

    http.begin(client,ip_port+path);      //Specify request destination
    http.addHeader("Content-Type", "application/json; charset=utf-8");  //Specify content-type header
    // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("{\"unique\":\"" +_id+"\", \"data\":\"" +data1+"\"}");   //Send the request
    // String payload = http.getString(); //Get the response payload
    DEBUG_PRINT(httpCode);
    http.end();  //Close connection

  }
    
}

void readSerialFromUno(){
  String buffer = readSerial();
  if (buffer.endsWith("\n")){
      buffer.trim();
      Processbuffer(buffer);
  }
}

String readSerial()
{
    String buffer;
    while (Serial.available())
    {
        char c = Serial.read();
        buffer.concat(c);
        delay(10);
    }
    Serial.flush();
    return buffer;
}

void Processbuffer(String buffer) {
  int ind1, ind2, ind3, ind4;
  String data1, data2, data3, data4;

  DEBUG_PRINT("Data from NANO: " + (String) buffer);
  ind1 = buffer.indexOf(',');  //finds location of first ','
  ind2 = buffer.indexOf(',', ind1 + 1);
  ind3 = buffer.indexOf(',', ind2 + 1);
  ind4 = buffer.indexOf(',', ind3 + 1);
  
  data1 = buffer.substring(0, ind1);  
  data2 = buffer.substring(ind1 + 1, ind2); 
  data3 = buffer.substring(ind2 + 1, ind3); 
  data4 = buffer.substring(ind3 + 1, ind4); 
  DEBUG_PRINT("id: " + (String)data1);
  DEBUG_PRINT("Data1: " + (String) data2);
  DEBUG_PRINT("Data2: " + (String) data3);
  DEBUG_PRINT("Data3: " + (String) data4);
  // buffer.substring(ind1 + 1, buffer.length()); 
  postData(data1,data2);
  buffer = "";
}

void connectToWifi(){
  WiFi.disconnect();
  WiFi.begin(ssid, password);
}