#ifndef SERIAL_HANDLING_H
#define SERIAL_HANDLING_H

#include <Arduino.h>

// Your code goes here
extern bool newData;

void recvWithStartEndMarkers(char* messageBuffer, const int bufferSize);
bool waitForCommand(const char* cmmdString, char* messageBuffer, const int bufferSize);
#endif // SERIAL_HANDLING_H