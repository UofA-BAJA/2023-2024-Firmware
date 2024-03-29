#include "logic.h"

#include <Arduino.h>
#include <HardwareSerial.h>
// #include <string>

#include "macros.h"
#include "enums.h"

#include "wifiTransmission.h"
///serial stuff
// Define startMarker and endMarker as preprocessor macros
#define startMarker '<'
#define endMarker '>'
const char numChars = 32;
char receivedChars[numChars];   // an array to store the received data
bool newData = false;
const char numOfOutboundChars = 50;
char outboundChars[numOfOutboundChars];   // an array to store the received data

////////////////////////////


//state stuff
enum WirelessTranscieverState {
    DONE_INITIALIZING,
    RESPOND_WITH_TYPE,
    WAIT_FOR_WIRELESS_CONNECTION,
    LISTEN_WIRELESSLY,
    WAIT_FOR_SERIAL_COMMAND
};

WirelessTranscieverState wireless_transciever_state = DONE_INITIALIZING; //initial state

#define LED_BUILTIN 2


void operatingProcedure() {
    switch (wireless_transciever_state)
    {
    case DONE_INITIALIZING: {
        pinMode(LED_BUILTIN, OUTPUT);

        Serial.println("Ready");
        Serial.flush();
        
        wireless_transciever_state = RESPOND_WITH_TYPE;
        break;
    }

    case RESPOND_WITH_TYPE: {
        if (waitForCommand(COMMANDS_SENDTYPE)) {
            Serial.println("LORA_PI");
            Serial.flush();
            
            wireless_transciever_state = WAIT_FOR_WIRELESS_CONNECTION;
        } 
        break;
    }

    case WAIT_FOR_WIRELESS_CONNECTION: {
        while (!connectClient()) {
            
        }
        FLASH_LED_TIMES(1);
        wireless_transciever_state = LISTEN_WIRELESSLY;
        break;
    }

    case LISTEN_WIRELESSLY: {
        int serial_printer_counter = 0;
        String output = readWirelesslySingleLine();

        if (output != "") {
            // DEBUG_PRINT("Received: ");
            FLASH_LED_TIMES(1);
            Serial.println(output);
            Serial.flush();

            // if (output == "RETRIEVE")

            if (output == "<{RETRIEVE}>") {
                wireless_transciever_state = WAIT_FOR_SERIAL_COMMAND;
                confirmCommandWirelessly("<STANDY_BY>");

            }
            else {
                confirmCommandWirelessly();
            }

        } else {
            serial_printer_counter++;
            delay(10);
        }

        if (serial_printer_counter > 1000) {
            DEBUG_PRINTLN("No data received for 10 seconds.");
            serial_printer_counter = 0;
        }

        break;
    }

    case WAIT_FOR_SERIAL_COMMAND: {
        TURN_LED_OFF;

        recvWithStartEndMarkers();

        char *ptr = strstr(receivedChars, "LISTENUP");

        if (newData) {
            DEBUG_PRINTLN("IN WAITING, TYPE LISTEN UP");

            newData = false;

            if (ptr != NULL) {
                DEBUG_PRINTLN("LISTENING UP");

                bool finished_recieving_datatypes = false;
                while (!finished_recieving_datatypes) {
                    recvWithStartEndMarkers();
                
                    if (newData) {
                        newData = false;
                        

                        if (strstr(receivedChars, "END") != NULL) {
                            finished_recieving_datatypes = true;
                        }
                        else {
                            strcat(outboundChars, receivedChars);
                            DEBUG_PRINTLN("ADDED TO OUTBOUND CHARS");
                        }
                    }
                }
                DEBUG_PRINTLN("SENDING OUTBOUND CHARS WIRELESSLY");

                printWirelessly(String(outboundChars));

                DEBUG_PRINTLN("going back to listening wirelessly");
                wireless_transciever_state = LISTEN_WIRELESSLY;
            } else {
                Serial.println("WRONG COMMAND");
                Serial.flush();
            }

        }
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
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
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
bool waitForCommand(const char* cmmdString) {
    
    recvWithStartEndMarkers();
    if (newData) {

        if (strcmp(receivedChars, cmmdString) == 0) {
            receivedChars[0] = '\0'; //clear received chars
            return true;
        }
        else if (receivedChars[0] != '\0')
        {
            DEBUG_PRINT("Currently is in state: ");
            DEBUG_PRINTLN(wireless_transciever_state);
            DEBUG_PRINT("Incorrect Command: ");
            DEBUG_PRINTLN(receivedChars);
        }
        else {
            // The receivedChars array is empty, so do not print "command not recognized"
        }

        newData = false;
    }
    

    return false;
}