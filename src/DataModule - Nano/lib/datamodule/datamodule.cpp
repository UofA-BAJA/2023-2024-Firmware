#include <avr/io.h>
#include <HardwareSerial.h>

#include "datamodule.h"

#include "sdcard.h"
#include "imu.h"
#include "rpm.h"
#include "config.h"


BAJA_EMBEDDED::DataModule::DataModule() {

}

void BAJA_EMBEDDED::DataModule::data_module_initialization_procedure() {
    InitializeSDCard();
}

void BAJA_EMBEDDED::DataModule::data_module_operating_procedure() {
    StartSDReading();

    bool logging = false;


    while(true){
        // Incoming command from raspberry pi!
        if(Serial.available() > 0){
            String command = Serial.readString();

            if(command == "Begin Logging"){
                logging = true;
            }

            else if(command == "End Logging"){
                logging = false;
                CloseSD();
            }
            
            else if(command == "Retrieve Logs"){
                if(logging){
                    Serial.println("You are still logging. stop logging first");
                }else{
                    SendFile();
                }
            }
        }
        
        if(logging){
            data_module_logging_procedure();
        }
        
        
    }
    

}


BAJA_EMBEDDED::DataModule* create_data_module_type() {
    
    initialize_data_module_select_pins();

    //read the select pins
    int data_module_select =((PINC & (1 << PINC2)) >> PINC2) << 2 | 
                            ((PINC & (1 << PINC1)) >> PINC1) << 1 | 
                            ((PINC & (1 << PINC0)) >> PINC0);

    DEBUG_PRINT("Data Module Select Pin Reads: ");
    DEBUG_PRINTLN(data_module_select);

    if (data_module_select == 0b111) {
        DEBUG_PRINTLN("RPM Module Initialized");
        return new RPM_DataModule;
    }
    else if(data_module_select == 0b110){
        DEBUG_PRINTLN("IMU Detected");
        return new IMU_DataModule;
    }
    else {
        DEBUG_PRINTLN("No datamodule found for this pin selection");
            
        return nullptr; 
    }

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

