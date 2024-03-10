#include <Arduino.h>
#include <WiFiMulti.h>
#include <WiFi.h>

WiFiMulti WiFiMulti;

// Use WiFiClient class to create TCP connections
WiFiClient client;

void initializeWifi() {
    // We start by connecting to a WiFi network
    WiFiMulti.addAP("yourAP", "yourPassword");

    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

bool connectToHost() {
    const uint16_t port = 80;
    const char * host = "192.168.4.1"; // ip or dns
    // const uint16_t port = 1337;
    // const char * host = "192.168.1.10"; // ip or dns

    Serial.print("Connecting to ");
    Serial.println(host);



    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        delay(333);
        return false;
    }
    return true;

}

void printWirelessly(String str) { //ik i shouldnt use strings but i am lazy
    if (client.connected()) {
        client.print(str);
    } else {
        Serial.println("Lost connection to the server. Waiting for reconnection...");

        initializeWifi();
        while (!connectToHost()) {};
        
        client.print(str); // Retry sending after successful reconnection
            
    }
}

String readWirelesssSingleLine() {
    int maxloops = 0;

    //wait for the server's reply to become available
    bool isConnected = false;

    // Wait for the server's reply to become available or for a timeout
    while (!client.available() && maxloops < 10000) {
        maxloops++;
        delay(1); // Delay 1 msec

        // Every 1000 loops check WiFi connection status
        if (maxloops % 1000 == 0) {
            // Check if WiFi is still connected
            if (WiFiMulti.run() != WL_CONNECTED) {
                Serial.println("WiFi disconnected. Attempting to reconnect...");

                // Re-initialize WiFi connection
                initializeWifi();

                // After reconnecting WiFi, try reconnecting to the host
                if (connectToHost()) {
                    Serial.println("Reconnected to the host. Continuing to wait for data...");
                } else {
                    Serial.println("Failed to reconnect to the host.");
                    return ""; // Exit if unable to reconnect to the host
                }
            }
        }
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