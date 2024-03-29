#include "logic.h"

#include <Arduino.h>
#include <HardwareSerial.h>
// #include <string>

#include "macros.h"
#include "enums.h"

#include "wifiTransmission.h"
///serial stuff
//wireless stuff
#define LEN_OF_DEVICE_NAME 7
#define BUFFER_SIZE 1024
#define WIRELESS_RESPONSE_TIMEOUT_MS 5000

char messageBuffer[BUFFER_SIZE] = MESSAGE_HEADERS_nxtdev WIRELESS_NODES_comput MESSAGE_HEADERS_mesg;
char nextDevice[LEN_OF_DEVICE_NAME]; // Buffer for the next device, plus null terminator


// Define startMarker and endMarker as preprocessor macros
#define startMarker '<'
#define endMarker '>'
const char numChars = 32;
char receivedChars[numChars];   // an array to store the received data
bool newData = false;
const char numOfOutboundChars = 50;
char outboundChars[numOfOutboundChars];   // an array to store the received data

////////////////////////////


// Define the wireless transceiver state enum

enum WirelessTranscieverState {
    DONE_INITIALIZING,
    WAITING_TO_RESPOND_WITH_TYPE,
    WAITING_FOR_WIRELESS_RESPONSE,
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
        if (waitForCommand(COMMANDS_SENDTYPE)) {
            Serial.println("LORA_PIT");
            Serial.flush();
            
            wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;
        } 
        break;
    }

    case (WAITING_FOR_WIRELESS_RESPONSE): {
        ReadWirelessIntoBufferWithTimeout(messageBuffer, BUFFER_SIZE, WIRELESS_RESPONSE_TIMEOUT_MS);
        DEBUG_PRINTLN("Received: ");
        DEBUG_PRINTLN(messageBuffer);

        getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);

        if (strcmp(nextDevice, WIRELESS_NODES_client)) {
            //the client sent a message to which its intended device is the client, that means it just wants an acknowledgement
            //the server will go back to listening to the computer for serial input after the acknowledgement
            //the client will go back to listening to the server for a message after the acknowledgement
            setDeviceInBufferTo(messageBuffer, WIRELESS_NODES_server);
            setMessageInBufferTo(messageBuffer, "acknowledged");
            printWirelessly(messageBuffer);

            DEBUG_PRINTLN("going to sending wireless state from waiting for response");
            wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;
        }
        else if (strcmp(nextDevice, WIRELESS_NODES_rasbpi)) {
            DEBUG_PRINTLN("going to sending wireless state from waiting for response");

            wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;
        }

        break;
    }

    // case SENDING_WIRELESS_MESSAGE: {
    //     getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);

    //     if (strcmp(nextDevice, WIRELESS_NODES_client)) {
    //         //the host wants the client to respond to its message, weird
    //         //the client wants the server to respond to its message
    //         setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_mesg, "verified"); //we'll verify we got the message

    //         //make the computer the next device so that the client starts waiting for serial commands from the computer
    //         setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_nxtdev, WIRELESS_NODES_comput);

    //     }

    //     else if (strcmp(nextDevice, WIRELESS_NODES_comput)) {
    //         //the host wants the computer to respond to its message
    //         DEBUG_PRINTLN("going into sending wireless state agin from sending wireless");
    //         DEBUG_PRINTLN(messageBuffer);
    //         wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;
    //     }

    //     DEBUG_PRINTLN("Sending message:");
    //     DEBUG_PRINTLN(messageBuffer);
    //     printWirelessly(messageBuffer);


    //     break;
    // }

    // case WAIT_FOR_WIRELESS_CONNECTION: {
    //     TURN_LED_ON;
    //     while (!connectClient()) {}
    //     TURN_LED_OFF;
    //     delay(500);
    //     TURN_LED_ON;
    //     wireless_transciever_state = LISTEN_WIRELESSLY;
    //     break;
    // }

    // case LISTEN_WIRELESSLY: {
    //     int serial_printer_counter = 0;
    //     String output = readWirelesslySingleLine();

    //     if (output != "") {
    //         // DEBUG_PRINT("Received: ");
    //         TURN_LED_OFF;
    //         Serial.println(output);
    //         Serial.flush();

    //         // if (output == "RETRIEVE")

    //         if (output == "<{RETRIEVE}>") {
    //             wireless_transciever_state = WAIT_FOR_SERIAL_COMMAND;
    //             confirmCommandWirelessly("<STANDY_BY>");

    //         }
    //         else {
    //             confirmCommandWirelessly();
    //         }

    //     } else {
    //         serial_printer_counter++;
    //         delay(10);
    //     }

    //     if (serial_printer_counter > 1000) {
    //         DEBUG_PRINTLN("No data received for 10 seconds.");
    //         serial_printer_counter = 0;
    //     }

    //     break;
    // }

    // case WAIT_FOR_SERIAL_COMMAND: {

    //     recvWithStartEndMarkers();

    //     char *ptr = strstr(receivedChars, "LISTENUP");

    //     if (newData) {
    //         DEBUG_PRINTLN("IN WAITING, TYPE LISTEN UP");

    //         newData = false;

    //         if (ptr != NULL) {
    //             DEBUG_PRINTLN("LISTENING UP");

    //             bool finished_recieving_datatypes = false;
    //             while (!finished_recieving_datatypes) {
    //                 recvWithStartEndMarkers();
                
    //                 if (newData) {
    //                     newData = false;
                        

    //                     if (strstr(receivedChars, "END") != NULL) {
    //                         finished_recieving_datatypes = true;
    //                     }
    //                     else {
    //                         strcat(outboundChars, receivedChars);
    //                         DEBUG_PRINTLN("ADDED TO OUTBOUND CHARS");
    //                     }
    //                 }
    //             }
    //             DEBUG_PRINTLN("SENDING OUTBOUND CHARS WIRELESSLY");

    //             printWirelessly(String(outboundChars));

    //             DEBUG_PRINTLN("going back to listening wirelessly");
    //             TURN_LED_ON;
    //             wireless_transciever_state = LISTEN_WIRELESSLY;
    //         } else {
    //             Serial.println("WRONG COMMAND");
    //             Serial.flush();
    //         }

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

        // Calculate the length of the existing message after the header
        size_t existingMessageLength = strlen(insertPosition);

        // Calculate the length of the new message to be inserted
        size_t newMessageLength = strlen(newMessage);

        // Calculate the total length after insertion
        size_t totalLength = (insertPosition - buffer) + newMessageLength + existingMessageLength;

        // Check if the total length exceeds the buffer size (consider the null terminator)
        if (totalLength + 1 > bufferSize) {
            // Handle the case where the new message + existing message would overflow the buffer
            Serial.println("Error: New message and existing message are too long to fit in the buffer.");
            return;
        }

        // Move the existing message to the end of the new message, including the null terminator
        // Ensure to move the data safely to avoid overlapping issues
        memmove(insertPosition + newMessageLength, insertPosition, existingMessageLength + 1); // +1 to include null terminator

        // Copy the new message into the buffer at the insert position
        memcpy(insertPosition, newMessage, newMessageLength);
    } else {
        // Handle the case where the header is not found in the buffer
        Serial.println("EdtingBuffer: Header not found in the buffer.");
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
        Serial.println("Printing buffer: Header not found in the buffer.");
    }
}

void setMessageInBufferTo(char* messageBuffer, const char* message) {
    setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_mesg, message);
}

void setDeviceInBufferTo(char* messageBuffer, const char* device) {
    setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_nxtdev, device);
}
