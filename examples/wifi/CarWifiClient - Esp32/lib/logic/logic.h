#ifndef LOGIC_H
#define LOGIC_H

#include <Arduino.h>

void operatingProcedure();

bool waitForCommand(const char* cmmdString);
void recvWithStartEndMarkers();
bool getNextDevice(const char* messageBuffer, char* output, size_t outputSize);
String convertToCommand(char* receivedChars);
void establishWirelessConnection();

void setTextAfterHeader(char* buffer, size_t bufferSize, const char* header, const char* newMessage);
void printTextAfterHeader(const char* buffer, const char* header);
void setDeviceAndMessageInBufferTo(const char* device, const char* message);

void resetMessageBuffer();

#endif // LOGIC_H