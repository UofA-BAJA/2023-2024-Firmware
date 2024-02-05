#ifndef CONFIG_H
#define CONFIG_H

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



#endif

