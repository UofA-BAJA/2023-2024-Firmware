#ifndef WIFITRANSMISSION_H
#define WIFITRANSMISSION_H

#include <Arduino.h>

void initializeWifi();

bool connectClient();

String readWirelesslySingleLine();

void confirmCommandWirelessly(String command = "<OK>"); 

void printWirelessly(String message);

#endif // WIFITRANSMISSION_H