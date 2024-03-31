#ifndef BUFFER_HANDLING_H
#define BUFFER_HANDLING_H

#include <Arduino.h>

#define LEN_OF_DEVICE_NAME 7
#define BUFFER_SIZE 1024
#define WIRELESS_RESPONSE_TIMEOUT_MS 10000

extern char inputmessageBuffer[BUFFER_SIZE];
extern char outputmessageBuffer[BUFFER_SIZE];
extern char nextDevice[LEN_OF_DEVICE_NAME];

void setTextAfterHeader(char* buffer, size_t bufferSize, const char* header, const char* newMessage);
void setDeviceAndMessageInBufferTo(char* buf, const char* device, const char* message);
void resetBuffer(char* buf);
bool isMessageMeantForDevice(char* buffer, const char* device);

#endif // BUFFER_HANDLING_H