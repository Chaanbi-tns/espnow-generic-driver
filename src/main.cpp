#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // nothing here
}