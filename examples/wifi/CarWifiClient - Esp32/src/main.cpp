//this is from https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientBasic/WiFiClientBasic.ino
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include "logic.h"


void setup() {
  Serial.begin(115200);
  delay(10);

}


void loop() {

  operatingProcedure();
  
}