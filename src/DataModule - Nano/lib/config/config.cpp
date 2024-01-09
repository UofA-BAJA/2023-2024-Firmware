#include "config.h"

#include <Arduino.h>

using namespace BAJA_EMBEDDED;

DEBUG_LEVEL debug_level;

void setup_debug() {
    switch (debug_level)
    {
    case (DEBUG_LEVEL::NONE):
        break;
    case (DEBUG_LEVEL::COMPLETE):
    case (DEBUG_LEVEL::GENERAL):
    case (DEBUG_LEVEL::CRITICAL):
        Serial.begin(9600);
        break;

    default:
        break;
    }
}