#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <rpm.h>
#include <config.h>

volatile int rpm_count = 0;

int main() {
  sei(); // Enable interrupts

  Serial.begin(9600);

  initialize_rpm_sensor();

  Serial.println("Initialized RPM sensor");

  _delay_ms(1000);
  
  while (1)
  {
    float rpm = calculate_rpm(rpm_count);

    Serial.print(rpm);
    _delay_ms(RPM_SENSING_DURATION_MS);
  }
  

  return 0;
}

ISR (INT0_vect) {
  rpm_count++;
}