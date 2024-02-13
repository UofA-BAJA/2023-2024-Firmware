import SerialDevices
import time
from util.ConfigParser import Commands, ModuleTypes
# ! lily was here

COMMANDS = {'b' : Commands.BEGIN,
            'e' : Commands.END,
            'r' : Commands.RETRIEVE,
            'q' : Commands.QUIT,
            'help' : Commands.HELP}


lora_dev = None

def main():

    serial_devices = SerialDevices.SerialDevices()

    # Get the lora device so we know how to send stuff back to the computer.
    lora_dev = serial_devices.get_device(ModuleTypes.LORA_PI)


    while True:
        command = input()
        serial_devices.execute_command(COMMANDS[command])



if __name__ == "__main__":
    main()