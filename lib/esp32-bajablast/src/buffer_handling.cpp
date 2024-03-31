#include "buffer_handling.h"

#include <Arduino.h>

#include "enums.h"
#include "macros.h"

char messageBuffer[BUFFER_SIZE] = MESSAGE_HEADERS_fart MESSAGE_HEADERS_nxtdev MESSAGE_HEADERS_mesg MESSAGE_HEADERS_bend;
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
void resetMessageBuffer() {
    // Ensure the buffer is clean before use
    memset(messageBuffer, 0, BUFFER_SIZE);

    // Calculate space left after adding MESSAGE_HEADERS_nxtdev
    size_t spaceLeft = BUFFER_SIZE - strlen(MESSAGE_HEADERS_nxtdev) - 1; // -1 for null terminator

    // Check if MESSAGE_HEADERS_mesg fits into the buffer alongside MESSAGE_HEADERS_nxtdev
    if (strlen(MESSAGE_HEADERS_mesg) < spaceLeft) {
        // Concatenate MESSAGE_HEADERS_nxtdev and MESSAGE_HEADERS_mesg into the buffer
        strcpy(messageBuffer, MESSAGE_HEADERS_fart);
        strcat(messageBuffer, MESSAGE_HEADERS_nxtdev);
        strcat(messageBuffer, MESSAGE_HEADERS_mesg);
        strcat(messageBuffer, MESSAGE_HEADERS_bend);
    } else {
        // Handle error: not enough space
        Serial.println("Error: Not enough space in buffer for both headers.");
    }
}

void setDeviceAndMessageInBufferTo(const char* device, const char* message) {
    resetMessageBuffer();

    setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_nxtdev, device);

    setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_mesg, message);
}


bool isMessageMeantForDevice(const char* device) {
    const char* nxtdevLocation = strstr(messageBuffer, MESSAGE_HEADERS_nxtdev);

    if (nxtdevLocation != nullptr) {
        const char* startOfInterest = nxtdevLocation + strlen(MESSAGE_HEADERS_nxtdev);
        
        if (startOfInterest - messageBuffer + LEN_OF_DEVICE_NAME <= BUFFER_SIZE) {
            char extractedDevice[LEN_OF_DEVICE_NAME + 1]; // +1 for null-termination
            strncpy(extractedDevice, startOfInterest, LEN_OF_DEVICE_NAME);
            extractedDevice[LEN_OF_DEVICE_NAME] = '\0'; // Ensure null termination

            if (strcmp(extractedDevice, device) == 0) {
                return true; // Device matches
            }
        }
    }
    return false; // Device does not match or 'nxtdev' not found
}

