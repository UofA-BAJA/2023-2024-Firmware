#include <rpm.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#include "config.h"
#include "datamodule.h"

#define RPM_SENSING_DURATION_MS 500

volatile int RPM_DataModule::left_rpm_counter = 0; // Define and initialize the static member

void RPM_DataModule::data_module_initialization_procedure() {
    sei(); // Enable interrupts
    initialize_left_rpm_sensor();

    if (debug_level == BAJA_EMBEDDED::DEBUG_LEVEL::COMPLETE) {
        Serial.println("initialized RPM data module");
    }
    
}

void RPM_DataModule::data_module_operating_procedure() {

    while (1)
    {
        /*operating procedure
        1. wait 
        */
    }
    
}

void RPM_DataModule::initialize_left_rpm_sensor() {

    // setting up rear left rpm sensor
    DDRD &= ~(1 << DDD2); // set direction for input
    PORTD |= (1 << PORTD2);  // enable the pullup resistor for stable input
    
    EICRA |= (1 << ISC01) | (1 << ISC00); // set INT0 to trigger on rising edge
    EIMSK |= (1 << INT0); // enable external interrupt for INT0
}

float calculate_rpm(int rpm_count) {

    float rpm; 

    rpm = (rpm_count * 60) / (RPM_SENSING_DURATION_MS / 1000.0);

    return rpm;
}

ISR (INT0_vect) {
    RPM_DataModule::left_rpm_counter++;
}