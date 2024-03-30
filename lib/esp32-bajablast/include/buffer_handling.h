#ifndef BUFFER_HANDLING_H
#define BUFFER_HANDLING_H

#define LEN_OF_DEVICE_NAME 7
#define BUFFER_SIZE 255
#define WIRELESS_RESPONSE_TIMEOUT_MS 10000

void setTextAfterHeader(char* buffer, size_t bufferSize, const char* header, const char* newMessage);
bool getNextDevice(const char* messageBuffer, char* output, size_t outputSize);
#endif // BUFFER_HANDLING_H