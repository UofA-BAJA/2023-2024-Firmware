#ifndef WIFITRANSMISSION_H
#define WIFITRANSMISSION_H

#include <Arduino.h>

void initializeWifi();

bool connectClient();

void ReadWirelessIntoBufferWithTimeout(char* buffer, size_t bufferSize, unsigned long timeout);


void printWirelessly(const char* buffer);

#endif // WIFITRANSMISSION_H