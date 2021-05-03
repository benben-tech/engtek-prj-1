#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <SensorLimit.h>
#include <Adafruit_ADS1X15.h> // download BusIO (Adafruit)
Adafruit_ADS1115 ads2;  /* Use this for the 16-bit version */

const char* ssid = "ShutdownValveAsm";
const char* password = "engtekprecision";

// Set your Static IP address
IPAddress local_IP(192, 168, 137, 52);
// Set your Gateway IP address
IPAddress gateway(192, 168, 137, 1);

IPAddress subnet(255, 255, 255, 0);

String path = "api/shutdownvalve/station-two/";
String ip_port = "http://192.168.137.6:8000/";

// #define PIN_LED D5
const int8_t sensor1=13;
const int8_t sensor2=12;
const int8_t sensor=16;
const int8_t PIN_LED=14;
String _latestID;
HTTPClient http;
int httpCode;

unsigned long id = 0;
int status = 0;

bool ready = true;

String _id,_status;

unsigned long _analogRead=0;

bool readOK = false;

// const int8_t PIN_CONTROL_BUTTON=2;

// SensorLimit sensorLimit(PIN_CONTROL_BUTTON);

void postData(unsigned long valve_id,unsigned int _analogread);
void connectToWifi(void);

void readSerialFromStation1(void);
void Processbuffer(String buffer);
String readSerial();

void OTA(void);

void getLatestID(void);

// void onClick(void);
// void onShortPress(void);
// void onLongPress(void);

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
   pinMode(sensor1, INPUT);
 pinMode(sensor2, INPUT);
 pinMode(sensor, INPUT);
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
  ads2.begin();
//   sensorLimit.setOnClickCallback(onClick);
//   sensorLimit.setOnShortPressCallback(onShortPress);
//   sensorLimit.setOnLongPressCallback(onLongPress);
//   sensorLimit.begin();
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
      // send data to station3 (dummy)
     Serial.println(String(_id) +","+String(_status)); // send data to station3
    }
    else if(id<0){
      Serial.println("-1,2"); // send data to station3(dummy)
    }
readSerialFromStation1();
  if(status == -1){ // if -1
    //send data to web
    //sendtoweb(id,""); send null value bcoz No Good
    ready = true;
  }
  else{
    ready = false;
  }
 }

 if((status == 1 ) && (ready == false)){
  if(digitalRead(sensor1) == LOW && digitalRead(sensor2) == LOW){ // sensors, down and pull actuator,active high
    delay(500);
    int sensorJudgement = digitalRead(sensor); // read height sensor 
    _analogRead = 0;
    if(sensorJudgement == LOW) { // good
      // read analog data (make a average of 5 samples
      // send to server 
        for (int i = 0; i < 5; i++) {
            _analogRead += ads2.readADC_SingleEnded(2);
            delay(10);
        }
        _analogRead = _analogRead/5;
        postData(id,_analogRead); // sendtoweb(id,data);
        ready = true;
    }
    else if(sensorJudgement == HIGH) { // no good
      // read analog data (make a average of 5 samples
      // send to server
        for (int i = 0; i < 5; i++) {
            _analogRead += ads2.readADC_SingleEnded(2);
            delay(10);
        }
        _analogRead = _analogRead/5;
        postData(id,_analogRead); // sendtoweb(id,data);
      status = -1;
      ready = true;
    }
  }
 }
 if((status == 2) && (ready == false)){
  //do nothing
  ready = true;
 }

}

// void onClick(void) {
//   ready = false;
// }
// void onShortPress(void) {
//   ready = false;
// }

// void onLongPress(void) {
//   DEBUG_PRINT("SEND DATA");
//   postData(id,_analogRead); // sendtoweb(id,data);
//   ready = true;
// }

void readSerialFromStation1(){
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
  DEBUG_PRINT("Data from Station1: " + (String) buffer);
  ind1 = buffer.indexOf(',');  //finds location of first ','
  ind2 = buffer.indexOf(',', ind1 + 1);

  _id = buffer.substring(0, ind1);  
  _status = buffer.substring(ind1 + 1, ind2); 

  id = _id.toInt();
  status = _status.toInt();
//   Serial.println(status);
  DEBUG_PRINT("id: " + (String)status);
  DEBUG_PRINT("id: " + (String)_id);
  DEBUG_PRINT("Status: " + (String) _status);
  buffer = "";
}

void postData(unsigned long valve_id,unsigned int _analogread){
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient

    http.begin(client,ip_port+path);      //Specify request destination
    http.addHeader("Content-Type", "application/json; charset=utf-8");  //Specify content-type header
    // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("{\"valve_id\":\"" +String(valve_id)+"\", \"data\":\"" +_analogread+"\"}");   //Send the request
    // String payload = http.getString(); //Get the response payload
    DEBUG_PRINT(httpCode);
    http.end();  //Close connection

  }
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