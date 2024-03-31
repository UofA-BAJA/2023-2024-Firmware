#include "logic.h"

#include <Arduino.h>
#include <HardwareSerial.h>
// #include <string>

//from BAJA library
#include "macros.h"
#include "enums.h"
#include <buffer_handling.h>
#include <serial_handling.h>


#include "wifiTransmission.h"
///serial stuff
#define WIRELESS_RESPONSE_TIMEOUT_MS 5000 // 5 seconds
#define WIRELESS_HEARTBEAT_FREQUENCY_MS 5000 // 5 seconds


bool isClientConnected = false;

// Define the wireless transceiver state enum

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
        initializeWifi();

        Serial.println("Ready");
        Serial.flush();
        
        wireless_transciever_state = WAITING_TO_RESPOND_WITH_TYPE;
        break;
    }

    case WAITING_TO_RESPOND_WITH_TYPE: {
        if (waitForCommand(COMMANDS_SENDTYPE, inputmessageBuffer, BUFFER_SIZE)) {
            Serial.println("LORA_PIT");
            Serial.flush();
            
            wireless_transciever_state = SMART_PASSTHROUGH;
        } 
        break;
    }


    case SMART_PASSTHROUGH:
        recvWithStartEndMarkers(inputmessageBuffer, BUFFER_SIZE);
        
        if (newData) {
            newData = false;
            Serial.flush();
            DEBUG_PRINT("Serially Received: ");
            DEBUG_PRINTLN(inputmessageBuffer);

            Serial.print("WSTATUS:");
            Serial.println(isClientConnected);
            Serial.flush();

            parseMessage(inputmessageBuffer);


        } else {
            // If no new data received, check if it's time to send a heartbeat
            static unsigned long lastHeartbeatTime = 0;

            unsigned long currentMillis = millis();
            if (currentMillis - lastHeartbeatTime >= WIRELESS_HEARTBEAT_FREQUENCY_MS) {
                lastHeartbeatTime = currentMillis;
                
                // Send heartbeat message to the computer
                // DEBUG_PRINTLN("Sending heartbeat to car...");
                setDeviceAndMessageInBufferTo(outputmessageBuffer, WIRELESS_NODES_server, "heartbeat");
                printWirelessly(outputmessageBuffer);   
                
                ReadWirelessIntoBufferWithTimeout(inputmessageBuffer, BUFFER_SIZE, WIRELESS_RESPONSE_TIMEOUT_MS);
                
                if (inputmessageBuffer[0] == '\0') {
                    isClientConnected = false;
                    DEBUG_PRINTLN("Client did not respond in time. Assuming disconnected.");
                }
                else {
                    DEBUG_PRINT("Wirelessly Received: ");
                    DEBUG_PRINTLN(inputmessageBuffer);
                }

                parseMessage(inputmessageBuffer);


            }
        }

        break;

    default:
        break;
    }
}

void parseMessageRecursive(char* message, char* end) {
    if (message >= end) {
        // Base case: if we've reached or surpassed the start of the message, stop.
        return;
    }

    // Find the last "!fart!" marker in the message, starting from 'end'.
    char* lastStart = nullptr;
    for (char* p = end; p >= message; --p) {
        if (strncmp(p, MESSAGE_HEADERS_start, 6) == 0) {
            lastStart = p;
            break;
        }
    }

    if (lastStart == nullptr) {
        // If no marker is found, we're done.
        return;
    }

    // Since we want to include "!fart!" in the message for rasbpi, startOfMessage points to "!fart!" itself.
    char* startOfMessage = lastStart; 
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

    
    // Handle the message.
    if (isMessageMeantForDevice(startOfMessage, WIRELESS_NODES_client)) {
        DEBUG_PRINTLN("Client is not supposed to send messages to itself");

    } else if (isMessageMeantForDevice(startOfMessage, WIRELESS_NODES_rasbpi)) {
        DEBUG_PRINTLN("Sending message meant for Raspberry Pi downhill wirelessly");
        printWirelessly(startOfMessage);

    } else if (isMessageMeantForDevice(startOfMessage, WIRELESS_NODES_comput)) {
        DEBUG_PRINTLN("Packet for the computer, sending serially");
        Serial.print("<");
        Serial.print(startOfMessage);
        Serial.print(">");
        Serial.println();
        Serial.flush();

    } else if (isMessageMeantForDevice(startOfMessage, WIRELESS_NODES_server)) {
        Serial.println("Message for the server, means the client wants an acknowledgement");
        setDeviceAndMessageInBufferTo(outputmessageBuffer, WIRELESS_NODES_client, "acknowledged");
        printWirelessly(outputmessageBuffer);
        isClientConnected = true;

    } else {
        Serial.println("No target device found, message is corrupted");
    }

    // Restore the character that was temporarily replaced with '\0'.
    *messageEnd = savedChar;

    // Recursively process the rest of the message, moving towards the start.
    parseMessageRecursive(message, nextEnd - 6); // Adjust to move before the "!fart!" marker processed.
    
}

// Wrapper function to simplify initial calls
void parseMessage(char* startOfMessage) {
    int messageLength = strlen(startOfMessage);
    parseMessageRecursive(startOfMessage, startOfMessage + messageLength);
}