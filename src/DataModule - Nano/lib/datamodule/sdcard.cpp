#include <SD.h>
#include <SPI.h>

#include "config.h"
#include "datamodule.h"

#define fileName "temp.csv"
#define chipSelect 10

File dataFile;

void InitializeSDCard(){
    if(!SD.begin(chipSelect)){
        Serial.println("Card failed, or not present");
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
    }
    dataFile.close();
    Serial.println("Finished");
}



void StartSDReading() {
    
    dataFile = SD.open(fileName, FILE_WRITE);

    if (dataFile) {
        DEBUG_PRINTLN("Data file successfully opened");
    }
    else{
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

void CloseSD(){
    dataFile.close();
    Serial.print("File Closed");
}
