#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "ShutdownValveAsm";
const char* password = "engtekprecision";

// Set your Static IP address
IPAddress local_IP(192, 168, 137, 53);
// Set your Gateway IP address
IPAddress gateway(192, 168, 137, 1);

IPAddress subnet(255, 255, 255, 0);

#define PIN_LED D5

String _latestID;


unsigned long id = 0;
int status = 0;

bool ready = true;

String _id,_status;

void connectToWifi(void);

void readSerialFromStation2(void);
void Processbuffer(String buffer);
String readSerial();

void OTA(void);

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
  if (!WiFi.config(local_IP, gateway, subnet)) {
    // Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    // Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  OTA();
}

void loop() {
  ArduinoOTA.handle();
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
  
  if(Serial.available() && (ready == true)){
    if(id == 0){
      // do nothing
    }
    else if(id>0){
      // send data to station4 
     Serial.println(String(_id) +","+String(_status)); // send data to station3
    }
    else if(id<0){
      Serial.println("-1,2"); // send data to station4(dummy)
    }
  readSerialFromStation2();
  if(status == -1){ // if -1
    //do nothing
    ready = true;
  }
  else{
    ready = false;
  }
 }
 if((status == 1) && (ready == false)){
  //do nothing
  ready = true;
 }
 if((status == 2) && (ready == false)){
  //do nothing
  ready = true;
 }
}

void readSerialFromStation2(){
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
  int ind1, ind2;
  DEBUG_PRINT("Data from Station2: " + (String) buffer);
  ind1 = buffer.indexOf(',');  //finds location of first ','
  ind2 = buffer.indexOf(',', ind1 + 1);
  
  _id = buffer.substring(0, ind1);  
  _status = buffer.substring(ind1 + 1, ind2); 

  id = _id.toInt();
  status = _status.toInt();
  DEBUG_PRINT("id: " + (String)_id);
  DEBUG_PRINT("Status: " + (String) _status);
  buffer = "";
}

void connectToWifi(){
  WiFi.disconnect();
  WiFi.begin(ssid, password);
}

void OTA(void){
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}