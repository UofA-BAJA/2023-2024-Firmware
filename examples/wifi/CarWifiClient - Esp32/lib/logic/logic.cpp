#include "logic.h"

#include <Arduino.h>
#include <HardwareSerial.h>

//from BAJA library
#include "macros.h"
#include "enums.h"
#include <buffer_handling.h>
#include <serial_handling.h>

#include "wifiTransmission.h"


enum WirelessTranscieverState {
    DONE_INITIALIZING,
    WAITING_TO_RESPOND_WITH_TYPE,
    SMART_PASSTHROUGH,
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
        if (waitForCommand(COMMANDS_SENDTYPE, inputmessageBuffer, BUFFER_SIZE)) {
            Serial.println("LORA_PI");
            Serial.flush();
            
            establishWirelessConnection();
        
            wireless_transciever_state = SMART_PASSTHROUGH;
        } 
        break;
    }

    case SMART_PASSTHROUGH: {
        recvWithStartEndMarkers(inputmessageBuffer, BUFFER_SIZE);

        if (isThereWirelessDataToRead()) {
            ReadWirelessIntoBufferWithTimeout(inputmessageBuffer, BUFFER_SIZE, WIRELESS_RESPONSE_TIMEOUT_MS);

            if (inputmessageBuffer[0] == '\0') {
                //received nothing
            } else {
                DEBUG_PRINT("Received message:");
                DEBUG_PRINTLN(inputmessageBuffer);
            }

            parseMessage(inputmessageBuffer);

        } else {
            if (newData) {
                DEBUG_PRINT("Received from pi: ");
                DEBUG_PRINTLN(inputmessageBuffer);
                newData = false;

                parseMessage(inputmessageBuffer);
            } 
        }
        
        break;
    }

    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////
//this function will not exit until it has a connection to the server and the device
void establishWirelessConnection() {
    tryToConnectToWifi();
    connectToHost();
}

//it modifies global variables, such as state
void parseMessage(char* startOfMessage) {
    char* end;
    char* start = strstr(startOfMessage, "fart");

    if (start == nullptr) {
        Serial.println("No start marker found");
        return;
    }
    // if (start != nullptr) {
    //     start += strlen(MESSAGE_HEADERS_start);
    //     end = strstr(start, MESSAGE_HEADERS_stop); // Find the end of the current message
    //     if (end != nullptr) {
    //         *end = '\0'; // Terminate the current message
    //     }
        
    //     // ... (rest of the isMessageMeantForDevice checks) ...

    //     if (end != nullptr) {
    //         end += strlen(MESSAGE_HEADERS_stop); // Move past the end marker
    //         if (*end != '\0') { // Check if there is more to parse
    //             parseMessage(end); // Recursively call parseMessage with the remaining message
    //         }
    //         *startOfMessage = '\0'; // Clear everything in the message that was just parsed
    //     }
    // }
    DEBUG_PRINTLN("Parsing message");
    if (isMessageMeantForDevice(start, WIRELESS_NODES_client)) {
        //means the server wants a response from the client, just tell the server its here
        DEBUG_PRINTLN("Server is asking for the client");
        setDeviceAndMessageInBufferTo(outputmessageBuffer, WIRELESS_NODES_server, "present");
        printWirelessly(outputmessageBuffer);

    } else if (isMessageMeantForDevice(start, WIRELESS_NODES_rasbpi)) {
        //probably is the rasberry pi wanting to send a message uphill, client is just a pass through
        //remember that client is connected to pi through serial
        DEBUG_PRINTLN("Rasberry pi is asking for the client");
        Serial.println(start);
        Serial.flush();

    } else if (isMessageMeantForDevice(start, WIRELESS_NODES_comput)) {
        //probably is the rasberry pi wanting to send a message uphill, client is just a pass through
        DEBUG_PRINTLN("Computer is asking for the client");
        printWirelessly(start);

    } else if (isMessageMeantForDevice(start, WIRELESS_NODES_server)) {
        //if you received a message intended for the server, that makes no sense, the server will not send a message to itself
        Serial.println("Server is not supposed to send messages to itself");

    } else {
        //this should never trigger, but if it does, it means the message is not meant for any device
        //packet is probably corrupted
        Serial.println("No target device found, message is corrupted");
    }
}