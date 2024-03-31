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

void parseMessage(char* message) {
    // Assuming message is null-terminated.
    char* currentPos = message;
    while (currentPos != nullptr && *currentPos != '\0') {
        // Find the start of the next "fart" marker.
        char* start = strstr(currentPos, "!fart!");
        if (start == nullptr) {
            Serial.println("No more start markers found");
            break; // Exit the loop if no more start markers are found.
        }

        // Find the end of the current message (next "fart" or end of string).
        char* end = strstr(start + 1, "!fart!");
        if (end == nullptr) {
            // If there's no other "!fart!" marker, we take the rest of the string.
            end = start + strlen(start);
        }

        // Temporarily mark the end of the current message.
        *end = '\0';

        // Handle the message.
        if (isMessageMeantForDevice(start, WIRELESS_NODES_client)) {
            DEBUG_PRINTLN("Client is not supposed to send messages to itself");

        } else if (isMessageMeantForDevice(start, WIRELESS_NODES_rasbpi)) {
            DEBUG_PRINTLN("Sending message meant for Raspberry Pi downhill wirelessly");
            printWirelessly(start);

        } else if (isMessageMeantForDevice(start, WIRELESS_NODES_comput)) {
            DEBUG_PRINTLN("Packet for the computer, sending serially");
            Serial.print("<");
            Serial.print(start);
            Serial.print(">");
            Serial.flush();

        } else if (isMessageMeantForDevice(start, WIRELESS_NODES_server)) {
            Serial.println("Message for the server, means the client wants an acknowledgement");
            setDeviceAndMessageInBufferTo(outputmessageBuffer, WIRELESS_NODES_client, "acknowledged");
            printWirelessly(outputmessageBuffer);
            isClientConnected = true;

        } else {
            Serial.println("No target device found, message is corrupted");
        }

        // Restore the character at the end of the current message and move past it for the next iteration.
        *end = '!';
        currentPos = end + 1;
    }
}