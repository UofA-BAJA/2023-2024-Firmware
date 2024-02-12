#include <SD.h>
#include <SPI.h>

#include "macros.h"
#include "datamodule.h"

#define fileName "temp.csv"
#define chipSelect 10

File dataFile;

void InitializeSDCard(){
    if(!SD.begin(chipSelect)){
        DEBUG_PRINTLN("Card failed, or not present");
        while(1);
    }

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
