#include <ArduinoJson.h>


void setup() {
Serial.begin(9600);
}

void loop() {
  StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
root["id"]="1";
root["value"]=1.14;
root.printTo(Serial);
}
