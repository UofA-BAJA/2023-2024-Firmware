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
                DEBUG_PRINTLN("Sending heartbeat to car...");
                setDeviceAndMessageInBufferTo(outputmessageBuffer, WIRELESS_NODES_client, "heartbeat");
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

void parseMessage(char* startOfMessage) {
    char* end;
    char* start = strstr(startOfMessage, "fart");

    if (start == nullptr) {
        Serial.println("No start marker found");
        return;
    }

    DEBUG_PRINTLN("Parsing message");
    if (isMessageMeantForDevice(start, WIRELESS_NODES_client)) {
        //if you received a message intended for the client from the client, that makes no sense, the cleitn will not send a message to itself
        DEBUG_PRINTLN("Client is not supposed to send messages to itself");

    } else if (isMessageMeantForDevice(start, WIRELESS_NODES_rasbpi)) {
        //probably is the computer wanting to send messages downhill, server is just a pass through
        DEBUG_PRINTLN("Computer is asking for the rasberry pi");
        printWirelessly(start);

    } else if (isMessageMeantForDevice(start, WIRELESS_NODES_comput)) {
        //client is sending a message that needs to be sent serially to the computer
        //just pass through the message
        DEBUG_PRINTLN("Computer is asking for the client");
        Serial.println(start);
        Serial.flush();

    } else if (isMessageMeantForDevice(start, WIRELESS_NODES_server)) {
        //if you received a message intended for the server, its the client wanting an acknowledgement
        Serial.println("Server is not supposed to send messages to itself");
        setDeviceAndMessageInBufferTo(outputmessageBuffer, WIRELESS_NODES_client, "acknowledged");
        printWirelessly(outputmessageBuffer);
        isClientConnected = true;

    } else {
        //this should never trigger, but if it does, it means the message is not meant for any device
        //packet is probably corrupted
        Serial.println("No target device found, message is corrupted");
    }
}
