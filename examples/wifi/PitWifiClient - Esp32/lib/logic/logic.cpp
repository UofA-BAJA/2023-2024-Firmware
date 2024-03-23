#include "logic.h"

#include <Arduino.h>
#include <HardwareSerial.h>

#include "macros.h"
#include "enums.h"

#include "wifiTransmission.h"
//wireless stuff
#define WIRELESS_RESPONSE_TIMEOUT_MS 1000
int wireless_response_timer = 0;
String response;
///////////////////////////


///serial stuff
// Define startMarker and endMarker as preprocessor macros
#define startMarker '<'
#define endMarker '>'
const char numChars = 32;
char receivedChars[numChars];   // an array to store the received data
bool newData = false;
////////////////////////////

//led stuff
#define LED_BUILTIN 2

//state stuff
enum WirelessTranscieverState {
    DONE_INITIALIZING,
    RESPOND_WITH_TYPE,
    ATTEMPT_WIRELESS_CONNECT,
    WAIT_FOR_SERIAL_COMMAND,
    WAIT_FOR_WIRELESS_CONFIRMATION,
    LISTEN_WIRELESSLY
};

WirelessTranscieverState wireless_transciever_state = DONE_INITIALIZING; //initial state

void operatingProcedure() {
    switch (wireless_transciever_state)
    {
    case DONE_INITIALIZING: {
        pinMode(LED_BUILTIN, OUTPUT);

        FLASH_LED_TIMES(1);

        Serial.println("Ready");
        Serial.flush();
        
        wireless_transciever_state = RESPOND_WITH_TYPE;
        break;
    }

    case RESPOND_WITH_TYPE: {
        if (waitForCommand(COMMANDS_SENDTYPE)) {
            Serial.println("LORA_PIT");
            Serial.flush();
            
            wireless_transciever_state = ATTEMPT_WIRELESS_CONNECT;
        } 
        break;
    }

    case ATTEMPT_WIRELESS_CONNECT: {
        initializeWifi(); //u dont have to keep this when porting over lora

        while (!connectToHost()) {}; //wait until it is connected to the server

        FLASH_LED_TIMES(1);

        wireless_transciever_state = WAIT_FOR_SERIAL_COMMAND;
        break;
    }

    case WAIT_FOR_SERIAL_COMMAND: {
        TURN_LED_OFF;
        recvWithStartEndMarkers();

        if (newData) {
            // Make the onboard LED blink
            TURN_LED_ON;
            Serial.println(receivedChars);
            Serial.flush();
            newData = false;

            // Send the received data to the server
            printWirelessly(convertToCommand(receivedChars));

            wireless_transciever_state = WAIT_FOR_WIRELESS_CONFIRMATION;
            wireless_response_timer = millis();
        }

        break;
    }

    case WAIT_FOR_WIRELESS_CONFIRMATION: {
        // Wait for the server's reply to become available
        response = readWirelesssSingleLine();
        if (response.length() > 0) {
            
            if (response == "<STANDBY>") {
                wireless_transciever_state = LISTEN_WIRELESSLY;
                DEBUG_PRINTLN("Entering listen mode");
            } 
            TURN_LED_OFF;
            Serial.println(response);
            Serial.flush();
            wireless_transciever_state = WAIT_FOR_SERIAL_COMMAND;
        }

        if (millis() - wireless_response_timer > WIRELESS_RESPONSE_TIMEOUT_MS) {
            Serial.println("Wireless response timeout");            
            FLASH_LED_TIMES(2);
            Serial.flush();
            wireless_transciever_state = WAIT_FOR_SERIAL_COMMAND;
        }
        
        break;
    }

    case LISTEN_WIRELESSLY: {
        int serial_printer_counter = 0;
        String output = readWirelesssSingleLine();

        if (output != "") {
            // DEBUG_PRINT("Received: ");
            FLASH_LED_TIMES(1);
            Serial.println(output);
            Serial.flush();
        } else {
            serial_printer_counter++;
            delay(10);
        }

        if (serial_printer_counter > 1000) {
            DEBUG_PRINTLN("No data received for 10 seconds.");
            serial_printer_counter = 0;
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

String convertToCommand(char* receivedChars) {
    String receivedString = String(receivedChars);
    String formattedString = "<{" + receivedString + "}>";
    return formattedString;
}