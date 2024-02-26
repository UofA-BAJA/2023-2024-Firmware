#include <avr/io.h>
#include <util/delay.h>
#include <HardwareSerial.h>
#include <SD.h>
#include <SPI.h>

#include "datamodule.h"
#include "macros.h"
#include "enums.h"

#include "imu.h"
#include "rpm.h"

// Define startMarker and endMarker as preprocessor macros
#define startMarker '<'
#define endMarker '>'

#define fileName "temp.csv"
#define chipSelect 10

File dataFile;
const char numChars = 32;
char receivedChars[numChars];   // an array to store the received data

bool newData = false;

// void recvWithEndMarker();

enum DataModuleState {
    SD_CARD_INITIALIZATION,
    DATAMODULE_SPECIFIC_INITIALIZATION,
    RESPOND_WITH_TYPE,
    WAIT_TO_START_LOGGING,
    LOG_DATA,
    WAIT_TO_SEND_FILE
};


BAJA_EMBEDDED::DataModule::DataModule() {
    //empty constructor
}

void BAJA_EMBEDDED::DataModule::data_module_operating_procedure() {
    DataModuleState data_module_state = SD_CARD_INITIALIZATION; //initial state

    while(1) {
        switch (data_module_state)
        {
        case SD_CARD_INITIALIZATION:
            InitializeSDCard();
            data_module_state = DATAMODULE_SPECIFIC_INITIALIZATION;

            break;
        
        case DATAMODULE_SPECIFIC_INITIALIZATION:
            data_module_setup_procedure();
            data_module_state = RESPOND_WITH_TYPE;
            DEBUG_PRINTLN("Data Module Initialized");
            break;
        
        case RESPOND_WITH_TYPE:

            while(Serial.available() == 0); //wait for input
            
            const char* cmmdString = COMMANDS_SENDTYPE;
            if (strcmp(receivedChars, cmmdString) == 0) {
                // Exact match found
                Serial.println(data_module_type);
            }

            DEBUG_PRINTLN("Data Module Type: ");
            break;

        case WAIT_TO_START_LOGGING:
            if (Serial.available() > 0) {
        

                // recvWithEndMarker();
                DEBUG_PRINTLN("read in input: ");
                DEBUG_PRINT(receivedChars);

                // if (serial_input == COMMANDS_BEGIN) {
                //     StartSDReading();
                //     data_module_state = LOG_DATA;
                //     DEBUG_PRINTLN("Started data logging");
                // }
                // else {
                //     DEBUG_PRINTLN("Invalid command");
                // }
            }
            else {
                DEBUG_PRINTLN("Waiting to start logging...");
                _delay_ms(5000);
            }
            
            
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

    _delay_us(10); //delay for 10us
    }


}
////////////////////////////////////////////////////////////////////////
/////////////////////////initialization stuff/////////////////////////

BAJA_EMBEDDED::DataModule* create_data_module_type() {
    
    initialize_data_module_select_pins();

    //read the select pins
    int data_module_select =((PINC & (1 << PINC2)) >> PINC2) << 2 | 
                            ((PINC & (1 << PINC1)) >> PINC1) << 1 | 
                            ((PINC & (1 << PINC0)) >> PINC0);

    DEBUG_PRINT("Data Module Select Pin Reads: ");
    DEBUG_PRINTLN(data_module_select);

    if (data_module_select == 0b111) {
        DEBUG_PRINTLN("RPM Module Detected");
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

////////////////////////////////////////////////////////////////////////
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}


////////////////////////////////////////////////////////////////////////
/////////////////////////sd card stuff//////////////////////////////////
void InitializeSDCard(){
    DEBUG_PRINT("Initializing SD card on PIN: ");
    DEBUG_PRINTLN(chipSelect);

    if(!SD.begin(chipSelect)){
        DEBUG_PRINTLN("Card failed, or not present");
        while(1);
    }

    DEBUG_PRINTLN("Card initialized");

    if (SD.exists(fileName)) {
        SD.remove(fileName);
    }

}

void SendFile(){

    dataFile = SD.open(fileName, FILE_READ);

    while (dataFile.available() > 0) {
        String buffer = dataFile.readStringUntil('\n');
        buffer.trim();
        Serial.println(buffer);
        Serial.flush();
    }
    dataFile.close();
    DEBUG_PRINTLN("Finished");
}



void StartSDReading() {
    
    dataFile = SD.open(fileName, FILE_WRITE);

    if (!dataFile) {
        DEBUG_PRINTLN("Failed to open file for writing");
        while(1);
    }
}

// void WriteDataToSD(String dataString){
//     if (dataFile){
//         dataFile.println(dataString);
//     }else{
//         Serial.println("error opening file");
//     }
// }

void CloseSDFile(){
    dataFile.close();
}

////////////////////////////////////////////////////////////////////////
