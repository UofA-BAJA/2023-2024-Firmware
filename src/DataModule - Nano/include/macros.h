#ifndef MACROS_H
#define MACROS_H

#include <HardwareSerial.h>
#include <util/delay.h>

#define NONE 0 //no serial output, note that this will siginicantly reduce the size of the binary (FLASH)
#define DEV 1 //all serial output, for development

#define DEBUG_LEVEL DEV

#if DEBUG_LEVEL == DEV
    // If we are in development mode, define DEBUG_PRINT to output the variable
    #define DEBUG_PRINT(variable) \
        Serial.print(variable); \
        Serial.flush(); \
        _delay_ms(10);

    #define DEBUG_PRINTLN(variable) \
        Serial.println(variable); \
        Serial.flush(); \
        _delay_ms(10);


#else
    // If we are not in development mode, define DEBUG_PRINT to do nothing
    #define DEBUG_PRINT(variable)
    #define DEBUG_PRINTLN(variable)
#endif



#endif

