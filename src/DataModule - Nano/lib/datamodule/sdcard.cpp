#include <SD.h>
#include <SPI.h>
#include <HardwareSerial.h>

#include "datamodule.h"

#define fileName "temp.csv"
#define chipSelect 10

File dataFile;

void InitializeSDCard(){
    if(!SD.begin(chipSelect)){
        Serial.println("Card failed, or not present");
        while(1);
    }

    Serial.println("SD Card Initialized");

    SD.remove(fileName);
}

void SendFile(){

    dataFile = SD.open(fileName, FILE_READ);

    while (dataFile.available() > 0) {
        String buffer = dataFile.readStringUntil('\n');
        buffer.trim();
        Serial.println(buffer);
    }
    dataFile.close();
    Serial.println("Finished");
}



void StartSDReading() {
    
    dataFile = SD.open(fileName, FILE_WRITE);

    if (dataFile) {
        Serial.println("Data file successfully opened");
    }
    else{
        Serial.println("Failed to open file for writing");
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

void CloseSD(){
    dataFile.close();
    Serial.print("File Closed");
}
