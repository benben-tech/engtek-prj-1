#include <Arduino.h>

void readSerialFromUno(void);
String readSerial();
void Processbuffer(String buffer);

void setup() {
  Serial.begin(9600);

}

void loop() {
  readSerialFromUno();
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

  Serial.println("Data from NANO: " + (String) buffer);
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
  Serial.println("1st data: " + data1);
  Serial.println("1st data: " + data2);
  Serial.println("1st data: " + data3);
  buffer = "";
}
