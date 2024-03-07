#include "logic.h"

#include <Arduino.h>

enum class State {
    INIT,
    CONNECTED,
    DISCONNECTED
};

void operatingProcedure() {
    Serial.println("Hello, world!");
}

void initializationProcedure() {
    
}