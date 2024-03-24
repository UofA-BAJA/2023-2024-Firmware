#ifndef MACROS_H
#define MACROS_H

#include <HardwareSerial.h>

#define NONE 0 //no serial output, note that this will siginicantly reduce the size of the binary (FLASH)
#define DEV 1 //all serial output, for development

#define DEBUG_LEVEL DEV

#if DEBUG_LEVEL == DEV
    // If we are in development mode, define DEBUG_PRINT to output the variable
    #define DEBUG_PRINT(variable) \
        Serial.print(variable); \
        Serial.flush();

    #define DEBUG_PRINTLN(variable) \
        Serial.println(variable); \
        Serial.flush();

#else
    // If we are not in development mode, define DEBUG_PRINT to do nothing
    #define DEBUG_PRINT(variable)
    #define DEBUG_PRINTLN(variable)
#endif

#define BUILTIN_LED 2

#define TURN_LED_ON digitalWrite(BUILTIN_LED, HIGH)
#define TURN_LED_OFF digitalWrite(BUILTIN_LED, LOW)


#endif

