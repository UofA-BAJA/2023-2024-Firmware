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
    HANDLE_WIRELESS_RESPONSE,
    WAITING_FOR_SERIAL_FROM_COMPUTER,
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
        if (waitForCommand(COMMANDS_SENDTYPE, messageBuffer, BUFFER_SIZE)) {
            Serial.println("LORA_PIT");
            Serial.flush();
            
            wireless_transciever_state = WAITING_FOR_SERIAL_FROM_COMPUTER;
        } 
        break;
    }

    case (HANDLE_WIRELESS_RESPONSE): {
        ReadWirelessIntoBufferWithTimeout(messageBuffer, BUFFER_SIZE, WIRELESS_RESPONSE_TIMEOUT_MS);
        
        if (messageBuffer[0] == '\0') {
            isClientConnected = false;
            DEBUG_PRINTLN("Client did not respond in time. Assuming disconnected.");
        }
        else {
            DEBUG_PRINT("Wirelessly Received: ");
            DEBUG_PRINTLN(messageBuffer);
        }

        if (isMessageMeantForDevice(WIRELESS_NODES_server)) {
            // DEBUG_PRINTLN("sending ack to client");

            //the client sent a message to which its intended device is the server, that means it just wants an acknowledgement
            //the server will go back to listening to the computer for serial input after the acknowledgement
            //the client will go back to listening to the server for a message after the acknowledgement
            setDeviceAndMessageInBufferTo(WIRELESS_NODES_client, "acknowledged");
            printWirelessly(messageBuffer);

            isClientConnected = true;
            wireless_transciever_state = WAITING_FOR_SERIAL_FROM_COMPUTER;
        }
        

        break;
    }

    case WAITING_FOR_SERIAL_FROM_COMPUTER:
        recvWithStartEndMarkers(messageBuffer, BUFFER_SIZE);
        
        if (newData) {
            newData = false;
            Serial.flush();
            DEBUG_PRINT("Serially Received: ");
            DEBUG_PRINTLN(messageBuffer);

            Serial.print("WSTATUS:");
            Serial.println(isClientConnected);
            Serial.flush();


            if (isMessageMeantForDevice(WIRELESS_NODES_rasbpi)) {
                //the computer sent a message that it wants at the pi, so the server will send this message to client and wait for a response
                printWirelessly(messageBuffer);
                wireless_transciever_state = HANDLE_WIRELESS_RESPONSE;
            }
        } else {
            // If no new data received, check if it's time to send a heartbeat
            static unsigned long lastHeartbeatTime = 0;

            unsigned long currentMillis = millis();
            if (currentMillis - lastHeartbeatTime >= WIRELESS_HEARTBEAT_FREQUENCY_MS) {
                lastHeartbeatTime = currentMillis;
                
                // Send heartbeat message to the computer
                DEBUG_PRINTLN("Sending heartbeat to car...");
                setDeviceAndMessageInBufferTo(WIRELESS_NODES_client, "heartbeat");
                printWirelessly(messageBuffer);
                
                wireless_transciever_state = HANDLE_WIRELESS_RESPONSE;

            }
        }

        break;

    default:
        break;
    }
}
