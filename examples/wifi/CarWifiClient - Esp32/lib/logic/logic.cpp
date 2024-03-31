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

            parseWirelessMessage(inputmessageBuffer);

        } else {
            if (newData) {
                DEBUG_PRINT("Received from pi: ");
                DEBUG_PRINTLN(inputmessageBuffer);
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

//it modifies global variables, such as state
void parseWirelessMessage(char* startOfMessage) {

    const char* start = strstr(startOfMessage, MESSAGE_HEADERS_start);
    if (start != nullptr) {
        start += strlen(MESSAGE_HEADERS_start); 
    }
    
    if (isMessageMeantForDevice(WIRELESS_NODES_client, start)) {
        //means the server wants a response from the client, just tell the server its here
        setDeviceAndMessageInBufferTo(outputmessageBuffer, WIRELESS_NODES_server, "present")
        printWirelessly(outputmessageBuffer);
    } else if (isMessageMeantForDevice(WIRELESS_NODES_rasbpi, start)) {
        //do something
    } else if (isMessageMeantForDevice(WIRELESS_NODES_comput, start)) {
        //do something
    } else if (isMessageMeantForDevice(WIRELESS_NODES_server, start)) {
        //do something
    } else {
        //do something
    }
}