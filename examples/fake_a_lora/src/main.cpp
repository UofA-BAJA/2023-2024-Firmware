#include <Arduino.h>
#include <serial_handling.h>
#include <enums.h>
// put function declarations here:
int myFunction(int, int);
char serialBuffer[100];

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.begin(115200);
  Serial.println("Ready");
}

void loop() {
  if (waitForCommand(COMMANDS_SENDTYPE, serialBuffer, 100)){
    Serial.println("LORA_PIT");
    Serial.flush();
  };
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

