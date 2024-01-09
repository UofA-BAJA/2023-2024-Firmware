#ifndef CONFIG_H
#define CONFIG_H

namespace BAJA_EMBEDDED {

    enum class DEBUG_LEVEL {
        COMPLETE,
        GENERAL,
        CRITICAL,
        NONE
    };

}

extern BAJA_EMBEDDED::DEBUG_LEVEL debug_level;

void setup_debug();


#endif

