class ModuleTypes:
    IMU = "IMU"
    RPM_FRONT = "RPM_FRONT"
    RPM_REAR = "RPM_REAR"
    RPM_ENGINE = "RPM_ENGINE"
    BRAK_PRESSURE = "BRK"
    STEER = "STEER"
    PEDAL_ANGLE = "PEDAL"
    LORA_PI = "LORA_PI"
    LORA_PIT = "LORA_PIT"

class Commands:
    BEGIN = "BEGIN"
    END = "END"
    RETRIEVE = "RETRIEVE"
    QUIT = "QUIT"
    SEND_TYPE = "SENDTYPE"
    HELP = "HELP"

class DataTypes:
    PITCH = "PITCH"
    ROLL = "ROLL"
    RPM_REAR_LEFT = "RPM_REAR_LEFT"
    RPM_REAR_RIGHT = "RPM_REAR_RIGHT"
    RPM_FRONT_LEFT = "RPM_FRONT_LEFT"
    RPM_FRONT_RIGHT = "RPM_FRONT_RIGHT"
    BRAKE_PRESSURE_REAR = "BRAKE_PRESSURE_REAR"
    BRAKE_PRESSURE_FRONT = "BRAKE_PRESSURE_FRONT"
    GAS_PEDAL_ANGLE = "GAS_ANGLE"
    BRAKE_PEDAL_ANGLE = "BRAKE_ANGLE"
    ENGINE_RPM = "ENGINE_RPM"
    SPEED = "SPEED"

class MessageHeaders:
    START = "!fart!"
    TARGET_DEVICE = "-nxtdev:"
    BODY = "-mesg:"
    END = "!bend!"

class WirelessNodeTypes:
    CLIENT = "client"
    SERVER = "server"
    COMPUTER = "comput"
    RASBERRYPI = "rasbpi"
