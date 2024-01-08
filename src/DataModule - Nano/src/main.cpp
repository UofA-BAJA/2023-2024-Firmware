#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <rpm.h>
#include <config.h>

int main() {
  sei(); // Enable interrupts

  Serial.begin(9600);

  initialize_rpm_sensor();

  Serial.println("Initialized RPM sensor");

  _delay_ms(1000);
  
  
  

  return 0;
}

ISR (INT0_vect) {
  rear_left_rpm_counter++;
}