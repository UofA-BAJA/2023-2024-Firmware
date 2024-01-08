#include <datamodule.h>

#include <avr/io.h>
#include <Arduino.h>

#include <config.h>

using namespace BAJA_EMBEDDED;

DataModule::DataModule() {
    // empty constructor
    this->debug_level = DEBUG_LEVEL::NONE;
}

DataModule::DataModule(DEBUG_LEVEL debug) {

    this->debug_level = debug;

    switch (debug)
    {
    case (DEBUG_LEVEL::NONE):
        break;
    case (DEBUG_LEVEL::COMPLETE):
    case (DEBUG_LEVEL::GENERAL):
    case (DEBUG_LEVEL::CRITICAL):
        Serial.begin(9600);
        break;

    default:
        break;
    }
}

void DataModule::self_identify_type_of_data_module() {
    //look at pinout csv files
    // Clear the bits DDC0, DDC1, and DDC2 in the Data Direction Register C (DDRC).
    // This configures the pins PC0, PC1, and PC2 as inputs.
    DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC2));
        
    // Set the bits PORTC0, PORTC1, and PORTC2 in the Port C Data Register (PORTC).
    // This enables the internal pull-up resistors for the pins PC0, PC1, and PC2.
    PORTC |= ( (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2));


    //read the select pins
    int data_module_select = ((2 << PINC2) | (1 << PINC1) | PINC0);

    if (this->debug_level == DEBUG_LEVEL::COMPLETE) {
        Serial.print("DataModule select pins read ");
        Serial.println(data_module_select);
    }

    switch (data_module_select)
    {
    case /* constant-expression */:
        /* code */
        break;
    
    default:
        break;
    }


}

