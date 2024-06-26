#include "buffer_handling.h"

#include <Arduino.h>

#include "enums.h"
#include "macros.h"

char inputmessageBuffer[BUFFER_SIZE] = MESSAGE_HEADERS_start MESSAGE_HEADERS_nxtdev MESSAGE_HEADERS_mesg MESSAGE_HEADERS_stop;
char outputmessageBuffer[BUFFER_SIZE] = MESSAGE_HEADERS_start MESSAGE_HEADERS_nxtdev MESSAGE_HEADERS_mesg MESSAGE_HEADERS_stop;

char nextDevice[LEN_OF_DEVICE_NAME];

void setTextAfterHeader(char* buffer, size_t bufferSize, const char* header, const char* newMessage) {
    // Find the header in the buffer
    char* headerLocation = strstr(buffer, header);
    if (headerLocation != nullptr) {
        // Calculate the position immediately after the header
        char* insertPosition = headerLocation + strlen(header);

        // Calculate the length of the existing message after the header
        size_t existingMessageLength = strlen(insertPosition);

        // Calculate the length of the new message to be inserted
        size_t newMessageLength = strlen(newMessage);

        // Calculate the total length after insertion
        size_t totalLength = (insertPosition - buffer) + newMessageLength + existingMessageLength;

        // Check if the total length exceeds the buffer size (consider the null terminator)
        if (totalLength + 1 > bufferSize) {
            // Handle the case where the new message + existing message would overflow the buffer
            Serial.println("Error: New message and existing message are too long to fit in the buffer.");
            return;
        }

        // Move the existing message to the end of the new message, including the null terminator
        // Ensure to move the data safely to avoid overlapping issues
        memmove(insertPosition + newMessageLength, insertPosition, existingMessageLength + 1); // +1 to include null terminator

        // Copy the new message into the buffer at the insert position
        memcpy(insertPosition, newMessage, newMessageLength);
    } else {
        // Handle the case where the header is not found in the buffer
        Serial.println("EdtingBuffer: Header not found in the buffer.");
    }
}

// Function to reset the message buffer
void resetBuffer(char* buf) {
    // Ensure the buffer is clean before use
    memset(buf, 0, BUFFER_SIZE);

    // Calculate space left after adding MESSAGE_HEADERS_nxtdev
    size_t spaceLeft = BUFFER_SIZE - strlen(MESSAGE_HEADERS_nxtdev) - 1; // -1 for null terminator

    // Check if MESSAGE_HEADERS_mesg fits into the buffer alongside MESSAGE_HEADERS_nxtdev
    if (strlen(MESSAGE_HEADERS_mesg) < spaceLeft) {
        // Concatenate MESSAGE_HEADERS_nxtdev and MESSAGE_HEADERS_mesg into the buffer
        strcpy(buf, MESSAGE_HEADERS_start);
        strcat(buf, MESSAGE_HEADERS_nxtdev);
        strcat(buf, MESSAGE_HEADERS_mesg);
        strcat(buf, MESSAGE_HEADERS_stop);
    } else {
        // Handle error: not enough space
        Serial.println("Error: Not enough space in buffer for both headers.");
    }
}

void setDeviceAndMessageInBufferTo(char* buf, const char* device, const char* message) {
    resetBuffer(buf);

    setTextAfterHeader(buf, BUFFER_SIZE, MESSAGE_HEADERS_nxtdev, device);

    setTextAfterHeader(buf, BUFFER_SIZE, MESSAGE_HEADERS_mesg, message);
}



bool isMessageMeantForDevice(char* buffer, const char* device) {
    //8 is the length of ':nxtdev-'
    const char* nxtdevLocation = strstr(buffer, MESSAGE_HEADERS_nxtdev);
    if (nxtdevLocation != nullptr) {
        // Move past the nxtdev header to the start of the device name
        const char* startOfInterest = nxtdevLocation + strlen(MESSAGE_HEADERS_nxtdev);
        // Find the end of the device name, assuming it ends at the start of the next section, which we'll say is '-mesg:'
        // Adjust this based on your actual message format
        const char* endOfInterest = strstr(startOfInterest, MESSAGE_HEADERS_mesg);

        if (endOfInterest != nullptr) {
            // Compare the device name directly using strncmp to ensure we don't read past the end of the device name
            size_t deviceNameLength = endOfInterest - startOfInterest;

            // Check if the lengths match and if the device names are equal up to the found length
            if (strlen(device) == deviceNameLength && strncmp(startOfInterest, device, deviceNameLength) == 0) {
                return true; // Device matches
            }
        }
    }
    return false; // Device does not match or 'nxtdev' not found
}


