#ifndef WIFITRANSMISSION_H
#define WIFITRANSMISSION_H

#include <Arduino.h>

void tryToConnectToWifi();

void connectToHost();

void ReadWirelessIntoBufferWithTimeout(char* buffer, size_t bufferSize, unsigned long timeout);

// String readWirelesssSingleLine();

void printWirelessly(const char* buffer);

bool isThereWirelessDataToRead();

#endif // WIFITRANSMISSION_H