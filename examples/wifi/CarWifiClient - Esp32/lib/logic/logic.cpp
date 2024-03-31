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
void parseMessageRecursive(char* message, char* end) {
    if (message >= end) {
        // Base case: if we've reached or surpassed the start of the message, stop.
        return;
    }

    // Find the last "fart" marker in the message, starting from 'end'.
    char* lastStart = nullptr;
    for (char* p = end; p >= message; --p) {
        if (strncmp(p, MESSAGE_HEADERS_start, 6) == 0) {
            lastStart = p;
            break;
        }
    }

    if (lastStart == nullptr) {
        // If no marker is found, we're done.
        Serial.println("No start marker found in the remaining message");
        return;
    }

    // Process the found message.
    char* startOfMessage = lastStart; // Include "fart" in the message.
    char* nextEnd = lastStart; // Next recursion will look before this point.

    // Find the end marker "!bend!" to include in the message.
    char* messageEnd = strstr(startOfMessage, MESSAGE_HEADERS_stop);
    if (messageEnd != nullptr) {
        // Adjust messageEnd to point to the character after "!bend!" to include it.
        messageEnd += 6; // Include "!bend!" in the message.
    } else {
        // If no "!bend!" is found, we use the original end.
        messageEnd = end;
    }

    // Temporarily terminate the current message for processing.
    char savedChar = *messageEnd;
    *messageEnd = '\0';
    
    if (isMessageMeantForDevice(startOfMessage, WIRELESS_NODES_client)) {
        //means the server is just sending the acknoledgement of the the server
        //dont respond to the server, just send the message to the rasberry pi
        DEBUG_PRINTLN("Server is just acknowledging the client");
        

    } else if (isMessageMeantForDevice(startOfMessage, WIRELESS_NODES_rasbpi)) {
        //probably is the rasberry pi wanting to send a message downhill, client is just a pass through
        //remember that client is connected to pi through serial
        DEBUG_PRINTLN("there is a packet for the rasbbery pi, sending downhill serially");
        Serial.print("<");
        Serial.print(startOfMessage);
        Serial.print(">");
        Serial.println();
        Serial.flush();

    } else if (isMessageMeantForDevice(startOfMessage, WIRELESS_NODES_comput)) {
        //probably is the rasberry pi wanting to send a message uphill, client is just a pass through
        DEBUG_PRINTLN("packet is for computer, sending uphill wirelessly");
        printWirelessly(startOfMessage);

    } else if (isMessageMeantForDevice(startOfMessage, WIRELESS_NODES_server)) {
        //if you received a message intended for the server, that means the server sent a message it wants a response to
        //means the server wants a response from the client, just tell the server its here
        DEBUG_PRINTLN("Server is asking for for client response");
        setDeviceAndMessageInBufferTo(outputmessageBuffer, WIRELESS_NODES_server, "present");
        printWirelessly(outputmessageBuffer);

    } else {
        //this should never trigger, but if it does, it means the message is not meant for any device
        //packet is probably corrupted
        Serial.println("No target device found, message is corrupted");
    }

    *messageEnd = savedChar;

    // Recursively process the rest of the message, moving towards the start.
    parseMessageRecursive(message, nextEnd - 6); // Adjust to move before the "fart" 
}

// Wrapper function to simplify initial calls
void parseMessage(char* startOfMessage) {
    int messageLength = strlen(startOfMessage);
    parseMessageRecursive(startOfMessage, startOfMessage + messageLength);
}