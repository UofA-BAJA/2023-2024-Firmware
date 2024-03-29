#include <Arduino.h>
#include <WiFiMulti.h>
#include <WiFi.h>

WiFiMulti WiFiMulti;

// Use WiFiClient class to create TCP connections
WiFiClient client;

void tryToConnectToWifi() {
    // We start by connecting to a WiFi network
    WiFiMulti.addAP("yourAP", "yourPassword");

    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");

    int retryDelay = 400; // Start with a delay of 100 milliseconds
    const int maxDelay = 5000; // Maximum delay of 5 seconds

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(retryDelay);
        retryDelay *= 2; // Double the delay for the next attempt
        if (retryDelay > maxDelay) retryDelay = maxDelay; // Cap the delay at maxDelay
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

void connectToHost() {
    const uint16_t port = 80;
    const char * host = "192.168.4.1"; // ip or dns
    // const uint16_t port = 1337;
    // const char * host = "192.168.1.10"; // ip or dns

    Serial.print("Connecting to ");
    Serial.println(host);

    int retryDelay = 100; // Start with a delay of 100 milliseconds
    const int maxDelay = 4000; // Maximum delay of 1.6 seconds

    while (!client.connect(host, port)) {
        Serial.println("Connection failed, retrying...");
        delay(retryDelay);
        retryDelay *= 2; // Double the delay for the next attempt
        if (retryDelay > maxDelay) retryDelay = maxDelay; // Cap the delay at maxDelay
    }
}

bool isConnectedToWifiAndDevice() {
    // Check if connected to WiFi
    if (WiFiMulti.run() == WL_CONNECTED) {
        // Check if connected to the host
        if (client.connected()) {
            return true; // Connected to both WiFi and the host
        } else {
            Serial.println("Not connected to the host.");
        }
    } else {
        Serial.println("Not connected to WiFi.");
    }
    return false; // Not connected to WiFi or the host
}

void printWirelessly(const char* buffer) { 
    // Define the message header we're looking for
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
        Serial.println("Lost connection to the server. Waiting for reconnection...");

        tryToConnectToWifi();
        connectToHost();
        
        client.print(buffer); // Retry sending after successful reconnection
            
    }
}


// String readWirelesssSingleLine(int original_time) {
//     // Ensure WiFi is connected before proceeding

//     int maxloops = 0;

//     //wait for the server's reply to become available
//     bool isConnected = false;

//     // Wait for the server's reply to become available or for a timeout
//     while (!client.available() && maxloops < 10000) {
//         maxloops++;
//         delay(1); // Delay 1 msec

//         // Every 1000 loops check WiFi connection status
//         if (maxloops % 1000 == 0) {
//             // Check if WiFi is still connected
//             if (WiFiMulti.run() != WL_CONNECTED) {
//                 Serial.println("WiFi disconnected. Attempting to reconnect...");

//                 // Re-initialize WiFi connection
//                 tryToConnectToWifi();

//                 // After reconnecting WiFi, try reconnecting to the host
//                 if (connectToHost()) {
//                     Serial.println("Reconnected to the host. Continuing to wait for data...");
//                 } else {
//                     Serial.println("Failed to reconnect to the host.");
//                     return ""; // Exit if unable to reconnect to the host
//                 }
//             }
//         }
//     }

//     if (client.available() > 0) {
//         //read back one line from the server
//         String line = client.readStringUntil('\r');
//         return line;
//     }

//     return "";
// }

// Function to read data into a buffer from a wireless connection
void ReadWirelessIntoBufferWithTimeout(char* buffer, size_t bufferSize, unsigned long timeout) {
    // Ensure the buffer is clean before use
    memset(buffer, 0, bufferSize);
    // Index for the current position in the buffer
    size_t index = 0;
    // Maximum index to write to, leaving space for null terminator
    size_t maxIndex = bufferSize - 1;

    unsigned long startWaitTime = millis();

    while (true) {
        while (client.available() > 0 && index < maxIndex) {
            int byte = client.read();
            if (byte == -1) break; // No more data
            buffer[index++] = (char)byte; // Store the byte
        }
        
        if (index > 0) { // Data was received
            break;
        }

        if (millis() - startWaitTime > timeout) {
            if (!isConnectedToWifiAndDevice()) {
                // If not connected, no point in continuing
                Serial.println("Connection lost. Retrying Connection");
                tryToConnectToWifi();
                connectToHost();
                // break;
            } else {
                // Still connected, reset the timer and wait again
                Serial.println("No data received, but still connected. Waiting again.");
                startWaitTime = millis(); // Reset waiting time
            }
        }

        delay(1); // Small delay to prevent a tight loop
    }

    buffer[index] = '\0'; // Null-terminate the buffer
}