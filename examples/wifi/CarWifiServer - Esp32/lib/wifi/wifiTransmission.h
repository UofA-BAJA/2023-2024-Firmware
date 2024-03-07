#ifndef WIFITRANSMISSION_H
#define WIFITRANSMISSION_H

#include <Arduino.h>

void initializeWifi();

void connectClient();

String readWirelesslySingleLine();

#endif // WIFITRANSMISSION_H