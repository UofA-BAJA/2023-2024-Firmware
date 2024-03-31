#include "logic.h"

#include <Arduino.h>
#include <HardwareSerial.h>

//from BAJA library
#include "macros.h"
#include "enums.h"
#include <buffer_handling.h>


#include "wifiTransmission.h"
//wireless stuff

///////////////////////////


enum WirelessTranscieverState {
    DONE_INITIALIZING,
    WAITING_TO_RESPOND_WITH_TYPE,
    WAITING_FOR_WIRELESS_RESPONSE,
    WAITING_FOR_SERIAL_FROM_PI,
};


WirelessTranscieverState wireless_transciever_state = DONE_INITIALIZING; //initial state

void operatingProcedure() {
    switch (wireless_transciever_state)
    {
    case DONE_INITIALIZING: {

        Serial.println("Ready");
        Serial.flush();
        
        wireless_transciever_state = WAITING_TO_RESPOND_WITH_TYPE;
        break;
    }

    case WAITING_TO_RESPOND_WITH_TYPE: {
        if (waitForCommand(COMMANDS_SENDTYPE)) {
            Serial.println("LORA_PI");
            Serial.flush();
            
            establishWirelessConnection();
        
            wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;
        } 
        break;
    }

    case WAITING_FOR_WIRELESS_RESPONSE: {
        ReadWirelessIntoBufferWithTimeout(messageBuffer, BUFFER_SIZE, WIRELESS_RESPONSE_TIMEOUT_MS);

        if (messageBuffer[0] == '\0') {
            //received nothing
        }
        else {
            DEBUG_PRINT("Received message:");
            DEBUG_PRINTLN(messageBuffer);
        }
        
        // parseMessage();
        
        break;
    }

    case WAITING_FOR_SERIAL_FROM_PI: {
        recvWithStartEndMarkers();

        if (isThereWirelessDataToRead()) {
            wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;

        } else {
            if (newData) {
                DEBUG_PRINT("Received from pi: ");
                DEBUG_PRINTLN(messageBuffer);
                newData = false;

                getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);

                if (strcmp(nextDevice, WIRELESS_NODES_comput)) {

                    printWirelessly(messageBuffer);
                    wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;
                }
            } 
        }
        
        break;
    }

    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////
/////////////////////////serial communication/////////////////////////
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                messageBuffer[ndx] = rc;
                ndx++;
                if (ndx >= BUFFER_SIZE) {
                    ndx = BUFFER_SIZE - 1;
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



////////////////////////////////////////////////////////////////////////
//this function will not exit until it has a connection to the server and the device
void establishWirelessConnection() {
    tryToConnectToWifi();
    connectToHost();
}

// bool getNextDevice(const char* messageBuffer, char* output, size_t outputSize) {

//     const char* nxtdevLocation = strstr(messageBuffer, "nxtdev:");
//     if (nxtdevLocation != nullptr) {
//         const char* startOfInterest = nxtdevLocation + strlen("nxtdev:");
        
//         if (startOfInterest - messageBuffer + 6 <= BUFFER_SIZE) {
//             strncpy(output, startOfInterest, 6); // Copy the next 6 chars
//             output[6] = '\0'; // Ensure null termination
//             return true;
//         }
//     }
//     return false; // Return false if 'nxtdev' not found or there's insufficient space
// }

bool isMessageMeantForDevice(const char* device) {
    
    getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);
    if (strcmp(nextDevice, device) == 0) {
        return true;
    }
    return false;
}


void setDeviceAndMessageInBufferTo(const char* device, const char* message) {
    resetMessageBuffer();

    setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_nxtdev, device);

    setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_mesg, message);
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


void changeStateOnMessage(char* startOfMessage) {
    if (isMessageMeantForDevice(WIRELESS_NODES_rasbpi)) {
        //message is intended for the computer, so we will print out the message data serially
        Serial.println(messageBuffer);
        wireless_transciever_state = WAITING_FOR_SERIAL_FROM_PI;
    } else if (isMessageMeantForDevice(WIRELESS_NODES_client)) {
        //message is sent from the server, and it is intended to end at the client, the server probably is just sending a heart beat message
        //send back a present message
        DEBUG_PRINTLN("Sending present message to server");
        setDeviceAndMessageInBufferTo(WIRELESS_NODES_server, "Present!");
        printWirelessly(messageBuffer);
    }
}
//it modifies global variables, such as state
void parseMessage() {


    
}