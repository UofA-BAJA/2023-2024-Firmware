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



// RPM sensor
extern int RPM_SENSING_DURATION_MS; // time in ms the sensor will accumalte pulses for before calculating RPM


#endif

