#include <avr/io.h>

#include "rpm.h"
#include "config.h"

void initialize_rpm_sensor() {
    DDRD &= ~(1 << DDD2); // set direction for input
    PORTD |= (1 << PORTD2);  // enable the pullup resistor for stable input
    
    EICRA |= ((1 << ISC01) | (1 << ISC00)); // set INT0 to trigger on rising edge
    
    EIMSK |= (1 << INT0); // enable INT0
}

float calculate_rpm(int rpm_count) {

    float rpm;

    rpm = (rpm_count * 60) / (RPM_SENSING_DURATION_MS / 1000.0);

    return rpm;
}