#ifndef ENUMS_H
#define ENUMS_H

#define MODULE_TYPES_IMU "IMU"
#define MODULE_TYPES_RPM_FRONT "RPM_FRONT"
#define MODULE_TYPES_RPM_REAR "RPM_REAR"
#define MODULE_TYPES_RPM_ENGINE "RPM_ENGINE"
#define MODULE_TYPES_BRK "BRK"
#define MODULE_TYPES_STEER "STEER"
#define MODULE_TYPES_PEDAL "PEDAL"
#define MODULE_TYPES_LORA_PI "LORA_PI"
#define MODULE_TYPES_LORA_PIT "LORA_PIT"

#define COMMANDS_BEGIN "BEGIN"
#define COMMANDS_END "END"
#define COMMANDS_RETRIEVE "RETRIEVE"
#define COMMANDS_HELP "HELP"
#define COMMANDS_QUIT "QUIT"
#define COMMANDS_SENDTYPE "SENDTYPE"

#define DATA_TYPES_PITC "PITC"
#define DATA_TYPES_ROLL "ROLL"
#define DATA_TYPES_RPM_REAR_LEFT "RPM_REAR_LEFT"
#define DATA_TYPES_RPM_REAR_RIGHT "RPM_REAR_RIGHT"
#define DATA_TYPES_RPM_FRONT_LEFT "RPM_FRONT_LEFT"
#define DATA_TYPES_RPM_FRONT_RIGHT "RPM_FRONT_RIGHT"
#define DATA_TYPES_BRAKE_PRESSURE_REAR "BRAKE_PRESSURE_REAR"
#define DATA_TYPES_BRAKE_PRESSURE_FRONT "BRAKE_PRESSURE_FRONT"
#define DATA_TYPES_GAS_ANGLE "GAS_ANGLE"
#define DATA_TYPES_BRAKE_ANGLE "BRAKE_ANGLE"
#define DATA_TYPES_ENGINE_RPM "ENGINE_RPM"

#define MESSAGE_HEADERS_nxtdev "-nxtdev:"
#define MESSAGE_HEADERS_mesg "-mesg:"
#define MESSAGE_HEADERS_fart "fart"
#define MESSAGE_HEADERS_bend "bend"

#define WIRELESS_NODES_client "client"
#define WIRELESS_NODES_server "server"
#define WIRELESS_NODES_comput "comput"
#define WIRELESS_NODES_rasbpi "rasbpi"

#endif // ENUMS_H
