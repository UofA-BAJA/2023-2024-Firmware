#include <Servo.h>

Servo myservo;  // Create a Servo object

int pos = 0;    // Variable to store the servo position

void setup() {
  myservo.attach(9);  // Attaches the servo on pin 9
}

void loop() {
  // Move the servo to 0 degrees
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(15);  // Adjust this delay to control the servo speed
  }

  delay(1000);  // Pause at 180 degrees

  // Move the servo back to 180 degrees
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
    delay(15);  // Adjust this delay to control the servo speed
  }

  delay(1000);  // Pause at 0 degrees
}

