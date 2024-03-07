//is from https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiAccessPoint/WiFiAccessPoint.ino

#include <Arduino.h>

#include "wifiTransmission.h"
#include "logic.h"

void setup() {

  Serial.begin(115200);
  Serial.flush();

  initializeWifi(); //dont include this in the lora code
  
}

void loop() {

  operatingProcedure();
}