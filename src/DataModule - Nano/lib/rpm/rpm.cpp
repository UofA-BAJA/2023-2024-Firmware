#include <avr/io.h>
#include <avr/interrupt.h>

#include <config.h>
#include <rpm.h>

float rear_left_rpm;
int rear_left_rpm_counter;



void rpm_data_module_initialization_procedure() {
    sei(); // Enable interrupts

    initialize_rpm_sensor();

}

void rpm_data_module_operating_procedure() {

    while (1)
    {
        /* code */
    }
    
}

void initialize_rpm_sensor() {

    // setting up rear left rpm sensor
    DDRB &= ~(1 << DDB0); // set direction for input
    PORTB |= (1 << PORTB0);  // enable the pullup resistor for stable input
    
    PCICR |= (1 << PCIE0); // enable pin change interrupt for PCINT7:0
    PCMSK0 |= (1 << PCINT0); // enable pin change interrupt for PCINT0
}

void start_counting_rpms() {

}

float calculate_rpm(int rpm_count) {

    float rpm; 

    rpm = (rpm_count * 60) / (RPM_SENSING_DURATION_MS / 1000.0);

    return rpm;
}

ISR (INT0_vect) {
  rear_left_rpm_counter++;
}