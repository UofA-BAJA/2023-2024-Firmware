#include <HardwareSerial.h>

int main() {
  Serial.begin(9600);


  Serial.println("Hello World");
  Serial.flush();
  for (int i = 0; i < 10; i++) {
    Serial.println(i);
  }

  return 0;
}
