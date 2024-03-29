#include "logic.h"

#include <Arduino.h>
#include <HardwareSerial.h>

#include "macros.h"
#include "enums.h"

#include "wifiTransmission.h"
//wireless stuff
#define LEN_OF_DEVICE_NAME 7
#define BUFFER_SIZE 255
#define WIRELESS_RESPONSE_TIMEOUT_MS 10000

char messageBuffer[BUFFER_SIZE] = MESSAGE_HEADERS_nxtdev MESSAGE_HEADERS_mesg;
//-nextdev:client-mesg:
char nextDevice[LEN_OF_DEVICE_NAME]; // Buffer for the next device, plus null terminator

bool onStartup = true;
///////////////////////////


///serial stuff
// Define startMarker and endMarker as preprocessor macros
#define startMarker '<'
#define endMarker '>'

bool newData = false;
////////////////////////////

//led stuff
#define LED_BUILTIN 2

//state stuff

enum WirelessTranscieverState {
    DONE_INITIALIZING,
    WAITING_TO_RESPOND_WITH_TYPE,
    WAITING_FOR_WIRELESS_RESPONSE,
    WAITING_FOR_SERIAL_FROM_PI,
};


WirelessTranscieverState wireless_transciever_state = DONE_INITIALIZING; //initial state

void operatingProcedure() {
    switch (wireless_transciever_state)
    {
    case DONE_INITIALIZING: {

        pinMode(LED_BUILTIN, OUTPUT);

        Serial.println("Ready");
        Serial.flush();
        
        wireless_transciever_state = WAITING_TO_RESPOND_WITH_TYPE;
        break;
    }

    case WAITING_TO_RESPOND_WITH_TYPE: {
        if (waitForCommand(COMMANDS_SENDTYPE)) {
            Serial.println("LORA_PI");
            Serial.flush();
            
            establishWirelessConnection();
        
            wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;
        } 
        break;
    }

    case WAITING_FOR_WIRELESS_RESPONSE: {
        ReadWirelessIntoBufferWithTimeout(messageBuffer, BUFFER_SIZE, WIRELESS_RESPONSE_TIMEOUT_MS);

        if (messageBuffer[0] == '\0') {
            // DEBUG_PRINTLN("sending message to client to verify connection");
        }
        else {
            DEBUG_PRINT("Received message:");
            DEBUG_PRINTLN(messageBuffer);
        }
        

        if (isMessageMeantForDevice(WIRELESS_NODES_rasbpi)) {
            //message is intended for the computer, so we will print out the message data serially
            Serial.println(messageBuffer);
            wireless_transciever_state = WAITING_FOR_SERIAL_FROM_PI;
        }
        else if (isMessageMeantForDevice(WIRELESS_NODES_client)) {
            //message is sent from the server, and it is intended to end at the client, the server probably is just sending a heart beat message
            //send back a present message
            DEBUG_PRINTLN("Sending present message to server");
            setDeviceAndMessageInBufferTo(WIRELESS_NODES_server, "Present!");
            printWirelessly(messageBuffer);
        }

        break;
    }

    case WAITING_FOR_SERIAL_FROM_PI: {
        recvWithStartEndMarkers();

        if (isThereWirelessDataToRead()) {
            wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;

        } else {
            if (newData) {
                DEBUG_PRINT("Received from pi: ");
                DEBUG_PRINTLN(messageBuffer);
                newData = false;

                getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);

                if (strcmp(nextDevice, WIRELESS_NODES_comput)) {

                    printWirelessly(messageBuffer);
                    wireless_transciever_state = WAITING_FOR_WIRELESS_RESPONSE;
                }
            } 
        }
        
        break;
    }
    // case SEND_SERIAL_TO_COMPUTER: {
        
    //     getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);

    //     if (strcmp(nextDevice, WIRELESS_NODES_comput)) {
    //         //this message is intended for the computer, so we will wait for the computer to respond
    //         setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_nxtdev, WIRELESS_NODES_client);
    //         Serial.print(messageBuffer);
    //         Serial.flush();
    //     }

    //     wireless_transciever_state = RECIEVING_SERIAL_FROM_COMPUTER;
    //     break;
    // }

    // case RECIEVING_SERIAL_FROM_COMPUTER: {
    //     recvWithStartEndMarkers();

    //     if (newData) {
    //         // DEBUG_PRINT("Received from computer: ");
    //         // DEBUG_PRINTLN(receivedChars);
    //         newData = false;

    //         getNextDevice(receivedChars, nextDevice, LEN_OF_DEVICE_NAME);

    //         setTextAfterHeader(messageBuffer, BUFFER_SIZE, MESSAGE_HEADERS_nxtdev, nextDevice);

    //         if (strcmp(nextDevice, WIRELESS_NODES_server)) {
    //             wireless_transciever_state = SENDING_WIRELESS_MESSAGE;

    //         }
    //     }
    //     break;
    // }

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

    const char* nxtdevLocation = strstr(messageBuffer, "nxtdev:");
    if (nxtdevLocation != nullptr) {
        const char* startOfInterest = nxtdevLocation + strlen("nxtdev:");
        
        if (startOfInterest - messageBuffer + 6 <= BUFFER_SIZE) {
            strncpy(output, startOfInterest, 6); // Copy the next 6 chars
            output[6] = '\0'; // Ensure null termination
            return true;
        }
    }
    return false; // Return false if 'nxtdev' not found or there's insufficient space
}

bool isMessageMeantForDevice(const char* device) {
    
    getNextDevice(messageBuffer, nextDevice, LEN_OF_DEVICE_NAME);
    if (strcmp(nextDevice, device) == 0) {
        return true;
    }
    return false;
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
        strcpy(messageBuffer, MESSAGE_HEADERS_nxtdev);
        strcat(messageBuffer, MESSAGE_HEADERS_mesg);
    } else {
        // Handle error: not enough space
        Serial.println("Error: Not enough space in buffer for both headers.");
    }
}