#ifndef BAJA_EMBEDDED_DATAMODULE_H
#define BAJA_EMBEDDED_DATAMODULE_H

#include <SD.h>

void SendFile();
void InitializeSDCard();
void StartSDReading();
// void WriteToSD(String dataString);
void CloseSDFile();

#endif // BAJA_EMBEDDED_DATAMODULE_H