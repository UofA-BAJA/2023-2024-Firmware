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
        if (waitForCommand(COMMANDS_SENDTYPE, messageBuffer, BUFFER_SIZE)) {
            Serial.println("LORA_PI");
            Serial.flush();
            
            establishWirelessConnection();
        
            wireless_transciever_state = SMART_PASSTHROUGH;
        } 
        break;
    }

    case SMART_PASSTHROUGH: {
        recvWithStartEndMarkers(messageBuffer, BUFFER_SIZE);

        if (isThereWirelessDataToRead()) {
            ReadWirelessIntoBufferWithTimeout(messageBuffer, BUFFER_SIZE, WIRELESS_RESPONSE_TIMEOUT_MS);

            if (messageBuffer[0] == '\0') {
                //received nothing
            } else {
                DEBUG_PRINT("Received message:");
                DEBUG_PRINTLN(messageBuffer);
            }

            parseMessage(messageBuffer);

        } else {
            if (newData) {
                DEBUG_PRINT("Received from pi: ");
                DEBUG_PRINTLN(messageBuffer);
                newData = false;

                
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
void parseMessage(char* startOfMessage) {


    
}