#ifndef WIFITRANSMISSION_H
#define WIFITRANSMISSION_H

#include <Arduino.h>

void initializeWifi();

bool connectToHost();

String readWirelesssSingleLine();

void printWirelessly(String str);

#endif // WIFITRANSMISSION_H