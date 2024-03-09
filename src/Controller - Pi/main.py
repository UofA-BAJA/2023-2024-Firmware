from utilsImporter import adding_utils_to_path

adding_utils_to_path()

from utils.SerialDevices import SerialDevices
from utils.ConfigParser import Commands, ModuleTypes

import time
import re
# ! lily was here
# ayo

COMMANDS = {'b' : Commands.BEGIN,
            'e' : Commands.END,
            'r' : Commands.RETRIEVE,
            'q' : Commands.QUIT,
            'help' : Commands.HELP}


lora_dev = None

def main():
    print("Starting up...")
    serial_devices = SerialDevices()

    if (ModuleTypes.LORA_PIT in serial_devices._serial_devices):
        serial_devices._serial_devices[ModuleTypes.LORA_PIT].close()

    lora_device = serial_devices.get_device(ModuleTypes.LORA_PI)

    if (lora_device) == None:

        print("No LORA PI DEVICE FOUND!!!!!!")
        while(1): pass
 
    while True:
        lora_serial_input = serial_devices._wait_for_lora_serial_input()

        command = parse_command(lora_serial_input)

        command_type_enum = None
        try:
            command_type_enum = Commands[command]
            # print(command_type_enum)
        except KeyError:
            print(f"No enum member with the name '{command}'")

        # command = input()
        serial_devices.sendCommandToAllDataModules(command_type_enum.name)


def parse_command(raw):
    # print(f"idk {command}")
    pattern = re.compile(r'{(.*?)}')
    # Find all matches in the text
    matches = pattern.findall(raw)

    return matches[0]

if __name__ == "__main__":
    main()