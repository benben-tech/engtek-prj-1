#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

// const char* ssid = "AREA51";
// const char* password = "ppoiiuYY778777";

const char* ssid = "ShutdownValveAsm";
const char* password = "engtekprecision";

// Set your Static IP address
IPAddress local_IP(192, 168, 137, 51);
// Set your Gateway IP address
IPAddress gateway(192, 168, 137, 1);

IPAddress subnet(255, 255, 255, 0);

String path = "api/shutdownvalve/station-one/";
String ip_port = "http://192.168.137.6:8000/";

// String path = "api/shutdownvalve/station-one/";
// String ip_port = "http://192.168.1.12:8000/";

#define PIN_LED D5
#define PROXIMITY D6

String _maxQty;

HTTPClient http;
int httpCode;

unsigned long id = 0;

String data1="0", data2, data3, data4;

bool checkMaxQty = true;
bool flagResetID = true;

void postDataCamera(unsigned long valve_id,String data1,String data2,String data3,String data4);
void connectToWifi(void);

void readSerialFromCamera(void);
void Processbuffer(String buffer);
String readSerial();
void serial_flush(void);

void OTA(void);

void getMaxQty(void);

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

  // if(checkMaxQty&&flagResetID){
  //   getMaxQty();
  //   checkMaxQty=false
  // }

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

  if(digitalRead(PROXIMITY)==0){
    readSerialFromCamera();
  }
  else if((digitalRead(PROXIMITY)== 1) && (data1 != "0")){
    
    if(id==0){
      getMaxQty();
      delay(250);
    }
    id++; // increment ID
    postDataCamera(id,data1,data2,data3,data4); // Send id,data1,data2,data3,data4 to web
    Serial.println(String(id)+","+data1); // data send to next station (station2)
    data1="0";
    data2="";
    data3="";
    data4="";

    if(id==_maxQty.toInt()){
      id=0;
    }
  }
  else if((digitalRead(PROXIMITY)== 1) && (data1 == "0") && (Serial.available())){
      serial_flush(); 
      Serial.println("-1,2");
  }
}

void postDataCamera(unsigned long valve_id,String data1,String data2,String data3,String data4){
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient

    http.begin(client,ip_port+path);      //Specify request destination
    http.addHeader("Content-Type", "application/json; charset=utf-8");  //Specify content-type header
    // http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
    int httpCode = http.POST("{\"valve_id\":\"" +String(valve_id)+"\", \"data_1\":\"" +data1+"\", \"data_2\":\"" +data2+"\", \"data_3\":\"" +data3+"\", \"data_4\":\"" +data4+"\"}"); 
   // String payload = http.getString(); //Get the response payload
    DEBUG_PRINT(httpCode);
    http.end();  //Close connection

  }
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

  DEBUG_PRINT("Data from Camera: " + (String) buffer);
  ind1 = buffer.indexOf(',');  //finds location of first ','
  ind2 = buffer.indexOf(',', ind1 + 1);
  ind3 = buffer.indexOf(',', ind2 + 1);
  ind4 = buffer.indexOf(',', ind3 + 1);
  
  data1 = buffer.substring(0, ind1);  // -1 or 1
  data2 = buffer.substring(ind1 + 1, ind2); 
  data3 = buffer.substring(ind2 + 1, ind3); 
  data4 = buffer.substring(ind3 + 1, ind4); 
  DEBUG_PRINT("Data1: " + (String) data1);
  DEBUG_PRINT("Data2: " + (String) data2);
  DEBUG_PRINT("Data3: " + (String) data3);
  DEBUG_PRINT("Data4: " + (String) data4);
  buffer = "";
}

void connectToWifi(){
  WiFi.disconnect();
  WiFi.begin(ssid, password);
}

void serial_flush(void) {
 while (true)
   {
   delay (40);  // give data a chance to arrive
   if (Serial.available ())
     {
     // we received something, get all of it and discard it
     while (Serial.available ())
       Serial.read();
     continue;  // stay in the main loop
     }
  else
    break;  // nothing arrived for 40 ms
   }
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

void getMaxQty(){
  if (WiFi.status() == WL_CONNECTED) {

    while(httpCode!=200){
        http.begin("http://192.168.1.12:8000/api/shutdownvalve/shutdownvalve_data_max_qty");
        httpCode = http.GET(); 
        if (httpCode > 0) {
          StaticJsonDocument<128> doc;

          DeserializationError error = deserializeJson(doc, http.getString());

          if (error) {
            DEBUG_PRINT("deserializeJson() failed: ");
            return;
          }
          
          JsonObject root_0 = doc[0];
          String root_0_max_qty = root_0["qty"]; // "25"
          // String root_0_max_qty = doc[0]["valve_id"];
          _maxQty=root_0_max_qty;
          http.end();   //Close connection  
        }
    }                                                 
  }
}