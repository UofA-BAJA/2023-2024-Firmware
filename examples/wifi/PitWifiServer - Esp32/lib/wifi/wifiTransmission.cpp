#include "wifiTransmission.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

WiFiServer server(80);
WiFiClient client;
// Use WiFiClient class to create TCP connections
// Set these to your desired credentials.
const char *ssid = "yourAP";
const char *password = "yourPassword";

void initializeWifi() {
    if (!WiFi.softAP(ssid, password)) {
        log_e("Soft AP creation failed.");
        while(1);
    }

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();

    Serial.println("Server started");
}

bool connectClient() {
    Serial.println("Trying for a client...");

    WiFiClient newClient = server.available(); // Check for incoming clients

    if (newClient) { // If a new client has connected,
        if (client.connected()) {
            // If there was a previous client connected, close that connection.
            Serial.println("Closing previous connection.");
            client.stop();
        }

        // Update the 'client' with the new connection
        client = newClient;
        Serial.println("New client connected.");
        return true; // Indicate that a new client has connected
    }

    // No new client has connected, return false.
    // No delay here; manage delay externally if needed, to control how often to check for clients.
    return false;
}

void ReadWirelessIntoBufferWithTimeout(char* buffer, size_t bufferSize, unsigned long timeout) {
    // Ensure the buffer is clean before use
    memset(buffer, 0, bufferSize);
    // Index for the current position in the buffer
    size_t index = 0;
    // Maximum index to write to, leaving space for null terminator
    size_t maxIndex = bufferSize - 1;

    unsigned long startWaitTime  = millis();
    
    while (true) {
        while (client.available() > 0 && index < maxIndex) {
            int byte = client.read();
            if (byte == -1) break; // No more data
            buffer[index++] = (char)byte; // Store the byte
        }
        
        if (index > 0) { // Data was received
            break;
        }

        // Periodically print status messages
        if (millis() - startWaitTime > timeout) {
            if (!client.connected()) {
                Serial.println("No client detected...");
                // Attempt to reconnect here if necessary
                connectClient();
            } 
                
            // Serial.println("Client response timed out...");
            
            break;
        }

        delay(5); // Small delay to prevent a tight loop
    }

    buffer[index] = '\0'; // Null-terminate the buffer
}

// String readWirelesslySingleLine() {
//     if (!client.connected()) {
//         while (!connectClient()) {}
//     }

//     String currentLine = "";              // to store the incoming line from the client
//     while (client.available()) {          // if there's bytes to read from the client,
//         char c = client.read();             // read a byte,
//         // Serial.write(c);                    // (optional) echo the character to the Serial port
//         if (c == '\n') {                    // if the byte is a newline character,
//             // Newline character marks the end of a line,
//             // so process or print the currentLine here
//             // Serial.println(currentLine);      // for example, print the line to the Serial port
//             currentLine = "";                 // then clear the currentLine for the next line
//         } else if (c != '\r') {             // if the byte is not a carriage return character,
//         currentLine += c;                 // add it to the currentLine
//         }
//     }

//     return currentLine;
// }

void printWirelessly(const char* buffer) { 
    // // Define the message header we're looking for
    // const char* messageHeader = ":mesg-";

    // // Find the message header in the buffer
    // const char* messageStart = strstr(buffer, messageHeader);
    
    // if (messageStart != nullptr) {
    //     // Adjust messageStart to point to the beginning of the actual message,
    //     // skipping over the length of messageHeader
    //     messageStart += strlen(messageHeader);
    // } else {
    //     // If the header isn't found, we'll print the whole buffer as a fallback
    //     messageStart = buffer;
    // }

    if (client.connected()) {
        client.print(buffer);
    } else {
        Serial.println("Client Disconnected. Reconnecting...");

        while (!connectClient()) {}; // Keep trying to reconnect until successful

        client.print(buffer); // Retry sending after successful reconnection
            
    }
}

void disconnectClient() {
    client.stop();
    Serial.println("Client Disconnected.");
}