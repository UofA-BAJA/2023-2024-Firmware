#ifndef WIFITRANSMISSION_H
#define WIFITRANSMISSION_H

#include <Arduino.h>

void initializeWifi();

void connectToHost();

String readWirelesssSingleLine();

#endif // WIFITRANSMISSION_H