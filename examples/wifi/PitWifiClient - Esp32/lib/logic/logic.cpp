#include "logic.h"

#include <Arduino.h>
#include <HardwareSerial.h>

#include "macros.h"
#include "enums.h"

#include "wifiTransmission.h"
//wireless stuff
#define LEN_OF_DEVICE_NAME 7
#define BUFFER_SIZE 1024
#define WIRELESS_RESPONSE_TIMEOUT_MS 5000

char messageBuffer[BUFFER_SIZE] = MESSAGE_HEADERS_nxtdev WIRELESS_NODES_client MESSAGE_HEADERS_mesg;
char nextDevice[LEN_OF_DEVICE_NAME]; // Buffer for the next device, plus null terminator
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
    SENDING_WIRELESS_MESSAGE,
    WAITING_FOR_WIRELESS_RESPONSE,
    SEND_SERIAL_TO_COMPUTER,
    RECIEVING_SERIAL_FROM_COMPUTER,
};


WirelessTranscieverState wireless_transciever_state = DONE_INITIALIZING; //initial state

void operatingProcedure() {
    switch (wireless_transciever_state)
    {
    case DONE_INITIALIZING: {
        tryToConnectToWifi(); //u dont have to keep this when porting over lora

        pinMode(LED_BUILTIN, OUTPUT);

        Serial.println("Ready");
        Serial.flush();
        
        wireless_transciever_state = RESPOND_WITH_TYPE;
        break;
    }

    case RESPOND_WITH_TYPE: {
        if (waitForCommand(COMMANDS_SENDTYPE)) {
            Serial.println("LORA_PIT");
            Serial.flush();
            
            wireless_transciever_state = SENDING_WIRELESS_MESSAGE;
        } 
        break;
    }

    case SENDING_WIRELESS_MESSAGE: {
        if (strstr(messageBuffer, WIRELESS_NODES_client) != NULL) {
            // WIRELESS_NODES_client was found in messageBuffer
            //this means that the message is intended for the client, which means that this is just a ping message to check if the client is still connected to the host
            establishWirelessConnection();

            wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;
            setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_mesg, "verify");
        }


        printWirelessly(messageBuffer);

        break;
    }

    case WAITING_FOR_WIRELESS_RESPONSE: {
        ReadWirelessIntoBufferWithTimeout(messageBuffer, BUFFER_SIZE, WIRELESS_RESPONSE_TIMEOUT_MS);

        printTextAfterHeader(messageBuffer, MESSAGE_HEADERS_mesg);

        getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);

        if (strcmp(nextDevice, WIRELESS_NODES_client)) {
            //this message is intended for the client, the host wants the client to respond with a message
        }
        else if (strcmp(nextDevice, WIRELESS_NODES_comput)) {
            //message is intended for the computer, so we will print out the message data serially
            wireless_transciever_state = SEND_SERIAL_TO_COMPUTER;
        }
    }

    // case ATTEMPT_WIRELESS_CONNECT: {
    //     TURN_LED_ON;
    //     while (!connectToHost()) {}; //wait until it is connected to the server
    //     TURN_LED_OFF;

    //     wireless_transciever_state = WAIT_FOR_SERIAL_COMMAND;
    //     break;
    // }

    // case WAIT_FOR_SERIAL_COMMAND: {
    //     recvWithStartEndMarkers();

    //     if (newData) {
    //         // Make the onboard LED blink
    //         Serial.println(receivedChars);
    //         Serial.flush();
    //         newData = false;

    //         // Send the received data to the server
    //         TURN_LED_ON;
    //         printWirelessly(convertToCommand(receivedChars));

    //         wireless_response_timer = millis();
    //         wireless_transciever_state = WAIT_FOR_WIRELESS_CONFIRMATION;
    //     }

    //     break;
    // }

    // case WAIT_FOR_WIRELESS_CONFIRMATION: {
    //     // Wait for the server's reply to become available
    //     response = readWirelesssSingleLine();
    //     if (response.length() > 0) {
    //         DEBUG_PRINT("Response:");
    //         Serial.println(response);
    //         Serial.flush();
    //         TURN_LED_OFF;

    //         if (response.indexOf("STAND") != -1) {
    //             TURN_LED_ON; 
    //             wireless_transciever_state = LISTEN_WIRELESSLY;
    //             DEBUG_PRINTLN("Entering listen mode");
    //             break;
    //         }
    //         else {
    //             DEBUG_PRINTLN("received stuff, now going to WAIT_FOR_SERIAL");
    //             wireless_transciever_state = WAIT_FOR_SERIAL_COMMAND;   
    //         }

    //         break;
    //     }
    //     else {
            
    //         DEBUG_PRINTLN("Wireless response timeout");            
    //         DEBUG_PRINTLN("timed out so going to WAIT_FOR_SERIAL");
    //         wireless_transciever_state = WAIT_FOR_SERIAL_COMMAND;
    //         break;
            
    //     }
        
        
    //     break;
    // }

    // case LISTEN_WIRELESSLY: {
    //     int serial_printer_counter = 0;
    //     String output = readWirelesssSingleLine();

    //     if (output != "") {
    //         TURN_LED_OFF; 
    //         // DEBUG_PRINT("Received: ");
    //         Serial.println(output);
    //         Serial.flush();
    //     } else {
    //         serial_printer_counter++;
    //         delay(10);
    //     }

    //     if (serial_printer_counter > 1000) {
    //         DEBUG_PRINTLN("No data received for 10 seconds.");
    //         serial_printer_counter = 0;
    //     }
    // }

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

////////////////////////////////////////////////////////////////////////
//this function will not exit until it has a connection to the server and the device
void establishWirelessConnection() {
    tryToConnectToWifi();
    connectToHost();
}

bool getNextDevice(const char* messageBuffer, char* output, size_t outputSize) {

    const char* nxtdevLocation = strstr(messageBuffer, "nxtdev");
    if (nxtdevLocation != nullptr) {
        const char* startOfInterest = nxtdevLocation + strlen("nxtdev");
        
        if (startOfInterest - messageBuffer + 6 <= BUFFER_SIZE) {
            strncpy(output, startOfInterest, 6); // Copy the next 6 chars
            output[6] = '\0'; // Ensure null termination
            return true;
        }
    }
    return false; // Return false if 'nxtdev' not found or there's insufficient space
}

void setTextAfterHeader(char* buffer, size_t bufferSize, const char* header, const char* newMessage) {
    // Find the header in the buffer
    char* headerLocation = strstr(buffer, header);
    if (headerLocation != nullptr) {
        // Calculate the position immediately after the header
        char* insertPosition = headerLocation + strlen(header);

        // Calculate the remaining space in the buffer, including space for the null terminator
        size_t remainingSpace = bufferSize - (insertPosition - buffer);
        if (strlen(newMessage) + 1 <= remainingSpace) { // +1 for null terminator
            // Safe to copy the new message into the buffer at the insert position
            strcpy(insertPosition, newMessage);
        } else {
            // Handle the case where the new message would overflow the buffer
            Serial.println("Error: New message is too long to fit in the buffer after the header.");
        }
    } else {
        // Handle the case where the header is not found in the buffer
        Serial.println("Error: Header not found in the buffer.");
    }
}

void printTextAfterHeader(const char* buffer, const char* header) {
    // Find the header in the buffer
    const char* headerLocation = strstr(buffer, header);
    if (headerLocation != nullptr) {
        // Calculate the start of the message content by skipping the header
        const char* messageStart = headerLocation + strlen(header);
        
        // Print the message content to Serial
        Serial.println(messageStart);
    } else {
        // If the header is not found, print an error message
        Serial.println("Error: Header not found in the buffer.");
    }
}
