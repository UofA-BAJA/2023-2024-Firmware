#include <datamodule.h>
#include <imu.h>
#include "rpm.h"

#include <avr/io.h>
#include <Arduino.h>

#include "config.h"


#include <SPI.h>
#include <SD.h>

BAJA_EMBEDDED::DataModule::DataModule() {

}

void BAJA_EMBEDDED::DataModule::SendFile(){

    dataFile = SD.open(fileName, FILE_READ);
    Serial.println(fileName);

    while (dataFile.available() > 0) {
        String buffer = dataFile.readStringUntil('\n');
        buffer.trim();
        Serial.println(buffer);
    }
    dataFile.close();
    Serial.println("Finished");
}

/* 
    Start of section for SD card reading
*/
void BAJA_EMBEDDED::DataModule::InitializeSDReading(int chipSelect, String fileName) {
    this->chipSelect = chipSelect;

    if(fileName.length() > 12){
        Serial.println("Your code won't work and life is terrible and please just make the file name less than 13 characters ong");
        while(1);
    }
    fileName.toUpperCase();
    this->fileName = fileName;
}

void BAJA_EMBEDDED::DataModule::StartSDReading() {
    if(!SD.begin(chipSelect)){
        Serial.println("Card failed, or not present");
        while(1);
    }

    Serial.println("SD Card Initialized");
    SD.remove(fileName);
    dataFile = SD.open(fileName, FILE_WRITE);

    if (dataFile) {
        Serial.println("Data file successfully opened");
    }
    else{
        while(1);
    }
}

void BAJA_EMBEDDED::DataModule::WriteToSD(String dataString){
    if (dataFile){
        dataFile.println(dataString);
    }else{
        Serial.println("error opening " + fileName);
    }
}

void BAJA_EMBEDDED::DataModule::CloseSD(){
    dataFile.close();
    Serial.print("File Closed");
}

/* 
    End of section for SD card reading
*/


BAJA_EMBEDDED::DataModule* create_data_module_type() {
    
    initialize_data_module_select_pins();

    //read the select pins
    int data_module_select =((PINC & (1 << PINC2)) >> PINC2) << 2 | 
                            ((PINC & (1 << PINC1)) >> PINC1) << 1 | 
                            ((PINC & (1 << PINC0)) >> PINC0);

    #if DEBUG_LEVEL == DEV
        Serial.print("DataModule select pins read: ");
        Serial.println(data_module_select);
    #endif
    
    if (data_module_select == 0b111) {
        Serial.println("RPM Sensor Detected");
        return new RPM_DataModule;
    }
    else if(data_module_select == 0b110){
        Serial.println("IMU Detected");
        return new IMU_DataModule;
    }
    else {
        #if DEBUG_LEVEL == DEV
            Serial.print("No datamodule found for this pin selection");
        #endif
     
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

