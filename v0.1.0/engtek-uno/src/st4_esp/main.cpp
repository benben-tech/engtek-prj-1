#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
// #include <ArduinoJson.h>
#include <ControlButton.h>
#include <Adafruit_ADS1X15.h> // download BusIO (Adafruit)
Adafruit_ADS1115 ads2;  /* Use this for the 16-bit version */

const int8_t PROXIMITY=12;
// ControlButton controlButton(PROXIMITY);

const char* ssid = "ShutdownValveAsm";
const char* password = "engtekprecision";

// Set your Static IP address
IPAddress local_IP(192, 168, 137, 54);
// Set your Gateway IP address
IPAddress gateway(192, 168, 137, 1);

IPAddress subnet(255, 255, 255, 0);

String path = "api/shutdownvalve/station-four/";
String ip_port = "http://192.168.137.6:8000/";

// #define PIN_LED D5
// #define sensor  D7 // sensor discrete value Good =HIGH, No good = LOW
const int8_t sensor=13;
const int8_t PIN_LED=14;
String _latestID;
HTTPClient http;
int httpCode;

unsigned long id = 0;
int status = 0;

bool ready = true;

String _id,_status;

unsigned long _analogRead=0;

void postData(unsigned long valve_id,unsigned int _analogread);
void connectToWifi(void);

void readSerialFromStation3(void);
void Processbuffer(String buffer);
String readSerial();

void OTA(void);

void getLatestID(void);

void onClick(void);
void onLongPress(void);

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
  pinMode(PROXIMITY, INPUT);
  pinMode(sensor, INPUT);
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
  // controlButton.setOnClickCallback(onClick);
  // controlButton.setOnLongPressCallback(onLongPress);
  // controlButton.begin();
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
    // digitalWrite(PIN_LED,!digitalRead(PIN_LED));
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
  readSerialFromStation3();
  if(status == -1){ // if -1
    //do nothing
    ready = true;
  }
  else{
    ready = false;
  }
 }
//  if((status == 1) && (ready == false)){
//     // controlButton.handle();
//     // _analogRead = ads2.readADC_SingleEnded(2);
      
//   if(digitalRead(PROXIMITY)==0){
//     postData(id,_analogRead);
//     ready = true;
//   }
//  }

  if((status == 1) && (ready == false)){
    if(digitalRead(PROXIMITY) == 0){
      _analogRead = 0;
      delay(1500); // debounce delay
      if(digitalRead(PROXIMITY) == 0){
      delay(1500); 
      int sensorJudgement = digitalRead(sensor);
      if(sensorJudgement == HIGH){ // Good
        // read analog data
        // after reading data send to webserver
        // sendtoweb(id,data);
        // for (int i = 0; i < 5; i++) {
        //     _analogRead += ads2.readADC_SingleEnded(2);
        //     delay(10);
        // }
        // _analogRead = _analogRead/5;
        _analogRead = ads2.readADC_SingleEnded(2);
        delay(50);
        postData(id,_analogRead); // sendtoweb(id,data);
        ready = true;
      }
      else if(sensorJudgement == LOW){ // NG
        // read analog data
        // after reading data send to webserver
        // sendtoweb(id,data);
        // for (int i = 0; i < 5; i++) {
        //     _analogRead += ads2.readADC_SingleEnded(2);
        //     delay(10);
        // }
        // _analogRead = _analogRead/5;
        _analogRead = ads2.readADC_SingleEnded(2);
        delay(50);
        postData(id,_analogRead); // sendtoweb(id,data);
        status = -1; // NG flag
        ready = true;
      }
    }
  }
 }


 if((status == 2) && (ready == false)){
  //do nothing
  ready = true;
 }

}

void onClick(void) {
  DEBUG_PRINT("DETECT");
}

void onLongPress(void) {
  DEBUG_PRINT("SEND DATA");
  postData(id,_analogRead); // sendtoweb(id,data);
  ready = true;
}

void readSerialFromStation3(){
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

// void getLatestID(){
//   if (WiFi.status() == WL_CONNECTED) {

//     while(httpCode!=200){
//         http.begin("http://192.168.137.6:8000/api/shutdownvalve/station-two/");
//         httpCode = http.GET(); 
//         if (httpCode > 0) {
//           StaticJsonDocument<128> doc;

//           DeserializationError error = deserializeJson(doc, http.getString());

//           if (error) {
//             DEBUG_PRINT("deserializeJson() failed: ");
//             return;
//           }
          
//           JsonObject root_0 = doc[0];
//           String root_0_valve_id = root_0["valve_id"]; // "25"
//           // String root_0_valve_id = doc[0]["valve_id"];
//           _latestID=root_0_valve_id;
//           DEBUG_PRINT("Data2: " + root_0_valve_id);
//           http.end();   //Close connection  
//         }
//     }                                                 
//     DEBUG_PRINT(httpCode);
//   }
// }

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
//   Serial.println("Ready");
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());
}