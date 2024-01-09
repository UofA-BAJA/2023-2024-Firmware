#include <datamodule.h>

#include <avr/io.h>
#include <Arduino.h>

#include "config.h"

#include "rpm.h"

BAJA_EMBEDDED::DataModule::DataModule() {

}

BAJA_EMBEDDED::DataModule* create_data_module_type() {
    
    initialize_data_module_select_pins();

    //read the select pins
    int data_module_select =((PINC & (1 << PINC2)) >> PINC2) << 2 | 
                            ((PINC & (1 << PINC1)) >> PINC1) << 1 | 
                            ((PINC & (1 << PINC0)) >> PINC0);

    if (debug_level == BAJA_EMBEDDED::DEBUG_LEVEL::COMPLETE) {
        Serial.print("DataModule select pins read: ");
        Serial.println(data_module_select);
    }
    

    if (data_module_select == 0b111) {
        return new RPM_DataModule;
    }
    else {
         if (debug_level <= BAJA_EMBEDDED::DEBUG_LEVEL::CRITICAL) {
        Serial.print("No datamodule found for this pin selection");
        }
     
        return nullptr; }

}

void initialize_data_module_select_pins() {
    //look at pinout csv files
    // Clear the bits DDC0, DDC1, and DDC2 in the Data Direction Register C (DDRC).
    // This configures the pins PC0, PC1, and PC2 as inputs.
    DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC2));
        
    // Set the bits PORTC0, PORTC1, and PORTC2 in the Port C Data Register (PORTC).
    // This enables the internal pull-up resistors for the pins PC0, PC1, and PC2.
    PORTC |= ( (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2));
}

