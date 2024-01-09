#include "config.h"

#include <Arduino.h>

#define NONE 0 //no serial output, note that this will siginicantly reduce the size of the binary (FLASH)
#define DEV 1 //all serial output, for development

#define DEBUG_LEVEL DEV

void setup_debug() {
    #if DEBUG_LEVEL == DEV
        Serial.begin(9600); // Initialize serial communication at 9600 baud rate
        Serial.println("Debug Mode: DEV");
    #endif
}
