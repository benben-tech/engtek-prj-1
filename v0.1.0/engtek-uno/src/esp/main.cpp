#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
const char* ssid     = "Demo WiFi";  
const char* password = "1234567890";

void postData(String val,String val2);
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
#endif



void setup() {
  WiFi.begin(ssid, password);
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN,LOW);
    static unsigned long wifiTimer = millis();
    const unsigned int wifiTimerInterval = 15000;
    /* Retry connection to WiFi every 15 seconds */
    if (millis() - wifiTimer >= wifiTimerInterval) {
      connectToWifi();
      wifiTimer = millis();
    }
  }else{
    digitalWrite(LED_BUILTIN,HIGH);
  }

}


void postData(String data1,String data2){
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
  
    HTTPClient http;    //Declare object of class HTTPClient

    http.begin("http://192.168.137.1:5000/api");      //Specify request destination
    http.addHeader("Content-Type", "application/json; charset=utf-8");  //Specify content-type header
    // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("{ \"valve\":\"" +data1+"\"}");   //Send the request
    String payload = http.getString();                  //Get the response payload
    
    http.end();  //Close connection

  }
    
}

void readSerialFromUno(){
  String buffer = readSerial();
  if (buffer.endsWith("\n")){
      int len = buffer.length();
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
  
  //buffer.toLowerCase();
//  Serial.println("Data from MEGA: " + (String) buffer);
  ind1 = buffer.indexOf(',');  //finds location of first ','
  ind2 = buffer.indexOf(',', ind1 + 1);
  ind3 = buffer.indexOf(',', ind2 + 1);
  ind4 = buffer.indexOf(',', ind3 + 1);
  
  data1 = buffer.substring(0, ind1);  
  data2 = buffer.substring(ind1 + 1, ind2); 
  data3 = buffer.substring(ind2 + 1, ind3); 
  data4 = buffer.substring(ind3 + 1, ind4); 
 
  buffer = "";
}

void connectToWifi(){
  WiFi.disconnect();
  WiFi.begin(ssid, password);
}