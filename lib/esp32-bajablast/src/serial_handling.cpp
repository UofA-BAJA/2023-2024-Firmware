#include "serial_handling.h"

#include <Arduino.h>

#include "macros.h"
#include "enums.h"

#define startMarker '<'
#define endMarker '>'

// Assume startMarker and endMarker are defined somewhere
// For example:
// const char startMarker = '<';
// const char endMarker = '>';

boolean newData = false;

void recvWithStartEndMarkers(char* messageBuffer, const byte bufferSize) {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                messageBuffer[ndx] = rc;
                ndx++;
                if (ndx >= bufferSize) {
                    ndx = bufferSize - 1;
                }
            }
            else {
                messageBuffer[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

/*this function will wait for a command to be received and will return a boolean if the command is successfuly recieved
    give the function a defined command from enums.h as input*/
bool waitForCommand(const char* cmmdString, char* messageBuffer, const byte bufferSize) {
    recvWithStartEndMarkers(messageBuffer, bufferSize);
    if (newData) {
        if (strcmp(messageBuffer, cmmdString) == 0) {
            memset(messageBuffer, 0, bufferSize); // Clear received chars
            newData = false; // Reset the flag
            return true;
        }
        else {
            DEBUG_PRINT("Currently in state: ");
            DEBUG_PRINTLN(wireless_transciever_state);
            DEBUG_PRINT("Incorrect Command: ");
            DEBUG_PRINTLN(messageBuffer);
            memset(messageBuffer, 0, bufferSize); // Clear received chars
        }

        newData = false; // Reset the flag regardless
    }

    return false;
}
