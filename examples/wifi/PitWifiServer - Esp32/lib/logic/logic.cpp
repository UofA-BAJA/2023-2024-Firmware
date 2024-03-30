#include "logic.h"

#include <Arduino.h>
#include <HardwareSerial.h>
// #include <string>

#include "macros.h"
#include "enums.h"

#include "wifiTransmission.h"
///serial stuff
#define WIRELESS_RESPONSE_TIMEOUT_MS 5000 // 5 seconds
#define WIRELESS_HEARTBEAT_FREQUENCY_MS 5000 // 5 seconds

//wireless stuff
#define LEN_OF_DEVICE_NAME 7
#define BUFFER_SIZE 255
char messageBuffer[BUFFER_SIZE] = MESSAGE_HEADERS_fart MESSAGE_HEADERS_nxtdev WIRELESS_NODES_comput MESSAGE_HEADERS_mesg MESSAGE_HEADERS_bend; // the -3 is to account for the start and end markers and null terminator
char nextDevice[LEN_OF_DEVICE_NAME]; // Buffer for the next device, plus null terminator
bool isClientConnected = false;

// Define startMarker and endMarker as preprocessor macros
#define startMarker '<'
#define endMarker '>'
// const char numChars = 32;
bool newData = false;


////////////////////////////


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
        if (waitForCommand(COMMANDS_SENDTYPE)) {
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
            DEBUG_PRINTLN("Client disconnected");
        }
        else {
            DEBUG_PRINT("Wirelessly Received: ");
            DEBUG_PRINTLN(messageBuffer);
        }

        getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);

        if (strcmp(nextDevice, WIRELESS_NODES_server)) {
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
        recvWithStartEndMarkers();
        
        if (newData) {
            newData = false;
            Serial.flush();
            DEBUG_PRINT("Serially Received: ");
            DEBUG_PRINTLN(messageBuffer);

            Serial.print("WSTATUS:");
            Serial.println(isClientConnected);
            Serial.flush();

            getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);

            if (strcmp(nextDevice, WIRELESS_NODES_rasbpi)) {
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
    //         wireless_transciever_state = HANDLE_WIRELESS_RESPONSE;
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

//this function is non-blocking if there is nothing on serial. It will only block if there is something on serial
void recvWithStartEndMarkers() {

    static boolean recvInProgress = false;
    static byte ndx = 0;
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                messageBuffer[ndx] = rc;
                ndx++;
                if (ndx >= BUFFER_SIZE) {
                    ndx = BUFFER_SIZE - 1;
                }
            }
            else {
                messageBuffer[ndx] = '\0'; // terminate the string
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

        if (strcmp(messageBuffer, cmmdString) == 0) {
            messageBuffer[0] = '\0'; //clear received chars
            return true;
        }
        else if (messageBuffer[0] != '\0')
        {
            DEBUG_PRINT("Currently is in state: ");
            DEBUG_PRINTLN(wireless_transciever_state);
            DEBUG_PRINT("Incorrect Command: ");
            DEBUG_PRINTLN(messageBuffer);
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
        
        if (startOfInterest -messageBuffer + 6 <= BUFFER_SIZE) {
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


void setDeviceAndMessageInBufferTo(const char* device, const char* message) {
    resetMessageBuffer();

    setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_nxtdev, device);

    setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_mesg, message);
}

// Function to reset the message buffer
void resetMessageBuffer() {
    // Ensure the buffer is clean before use
    memset(messageBuffer, 0, BUFFER_SIZE);

    // Calculate space left after adding MESSAGE_HEADERS_nxtdev
    size_t spaceLeft = BUFFER_SIZE - strlen(MESSAGE_HEADERS_nxtdev) - 1; // -1 for null terminator

    // Check if MESSAGE_HEADERS_mesg fits into the buffer alongside MESSAGE_HEADERS_nxtdev
    if (strlen(MESSAGE_HEADERS_mesg) < spaceLeft) {
        // Concatenate MESSAGE_HEADERS_nxtdev and MESSAGE_HEADERS_mesg into the buffer
        strcpy(messageBuffer, MESSAGE_HEADERS_fart);
        strcat(messageBuffer, MESSAGE_HEADERS_nxtdev);
        strcat(messageBuffer, MESSAGE_HEADERS_mesg);
        strcat(messageBuffer, MESSAGE_HEADERS_bend);
    } else {
        // Handle error: not enough space
        Serial.println("Error: Not enough space in buffer for both headers.");
    }
}