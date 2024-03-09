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
    Serial.println("Waiting for a client...");
    int max_attempts = 100;
    int attempts = 0;
    while (attempts < max_attempts) { // Infinite loop to wait for client
        client = server.accept(); // Listen for incoming clients

        if (client) { // If you get a client,
            Serial.println("New Client."); // Print a message out the serial port
            return true; // Return true as soon as a client connects
        }

        // Optional: delay to prevent the loop from running too fast,
        // which might be useful to reduce CPU usage on some systems.
        delay(333);
        attempts++;
    }
    // The function will never reach this point because of the infinite loop,
    // but a return statement is needed to avoid compiler warnings.
    return false;
}

String readWirelesslySingleLine() {
    String currentLine = "";              // to store the incoming line from the client
    while (client.available()) {          // if there's bytes to read from the client,
        char c = client.read();             // read a byte,
        Serial.write(c);                    // (optional) echo the character to the Serial port
        if (c == '\n') {                    // if the byte is a newline character,
            // Newline character marks the end of a line,
            // so process or print the currentLine here
            Serial.println(currentLine);      // for example, print the line to the Serial port
            currentLine = "";                 // then clear the currentLine for the next line
        } else if (c != '\r') {             // if the byte is not a carriage return character,
        currentLine += c;                 // add it to the currentLine
        }
    }

    return currentLine;
}

void printWirelessly(String str) { //ik i shouldnt use strings in cpp projects but i am lazy
    client.print(str);
}

void disconnectClient() {
    client.stop();
    Serial.println("Client Disconnected.");
}