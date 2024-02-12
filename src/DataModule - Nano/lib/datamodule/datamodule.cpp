#include <avr/io.h>
#include <HardwareSerial.h>

#include "datamodule.h"

#include "macros.h"
#include "enums.h"

#include "sdcard.h"
#include "imu.h"
#include "rpm.h"

enum DataModuleState {
    SD_CARD_INITIALIZATION,
    DATAMODULE_SPECIFIC_INITIALIZATION,
    WAIT_TO_START_LOGGING,
    LOG_DATA,
    WAIT_TO_SEND_FILE
};

DataModuleState data_module_state = SD_CARD_INITIALIZATION;

BAJA_EMBEDDED::DataModule::DataModule() {
    //empty constructor
}



void BAJA_EMBEDDED::DataModule::data_module_operating_procedure() {
    

    while(1) {
        switch (data_module_state)
        {
        case SD_CARD_INITIALIZATION:
            InitializeSDCard();
            data_module_state = DATAMODULE_SPECIFIC_INITIALIZATION;

            DEBUG_PRINTLN("Card initialized");
            break;
        
        case DATAMODULE_SPECIFIC_INITIALIZATION:
            data_module_setup_procedure();
            data_module_state = WAIT_TO_START_LOGGING;
            DEBUG_PRINTLN("Data Module Initialized");
            break;
        
        case WAIT_TO_START_LOGGING:
            if (Serial.available() > 0) {
                String serial_input = Serial.readString();

                if (serial_input == COMMANDS_BEGIN) {
                    StartSDReading();
                    data_module_state = LOG_DATA;
                    DEBUG_PRINTLN("Started data logging");
                }
            }
            
            DEBUG_PRINTLN("Waiting to start logging...");
            break;

        case LOG_DATA:
            if (Serial.available() > 0) {
                String serial_input = Serial.readString();

                if (serial_input == COMMANDS_END) {
                    CloseSDFile();
                    data_module_state = WAIT_TO_SEND_FILE;
                    DEBUG_PRINTLN("Stopped data logging...");

                }
                else {
                    data_module_logging_procedure();
                }
            }
            break;

        case WAIT_TO_SEND_FILE:
            if (Serial.available() > 0) {
                String serial_input = Serial.readString();

                if (serial_input == COMMANDS_RETRIEVE) {
                    SendFile();
                    data_module_state = WAIT_TO_START_LOGGING;
                }
            }
            break;
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

