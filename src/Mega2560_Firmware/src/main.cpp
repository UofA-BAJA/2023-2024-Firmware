#include <Arduino.h>

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void loop() {
  // check if there's any serial available:
  while (Serial.available() > 0) {
    // read the incoming byte:
    char incomingByte = Serial.read();
    
    // echo the byte back out the serial port:
    Serial.write(incomingByte);
  }
}
