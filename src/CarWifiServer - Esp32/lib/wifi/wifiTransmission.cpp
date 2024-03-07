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

void connectClient() {
    client = server.accept();   // listen for incoming clients
    if (client) {                             // if you get a client,
        Serial.println("New Client.");           // print a message out the serial port
        String currentLine = "";
    }
}

void printWirelessly(String str) { //ik i shouldnt use strings in cpp projects but i am lazy
    client.print(str);
}

void disconnectClient() {
    client.stop();
    Serial.println("Client Disconnected.");
}

String readWirelesssSingleLine() {
    int maxloops = 0;

    //wait for the server's reply to become available
    while (!client.available() && maxloops < 1000) {
        maxloops++;
        delay(1); //delay 1 msec
    }

    if (client.available() > 0) {
        //read back one line from the server
        String line = client.readStringUntil('\r');
        return line;
    } else {
        Serial.println("client.available() timed out ");
    }

    return "";
}