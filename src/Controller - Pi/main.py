from utilsImporter import adding_utils_to_path

adding_utils_to_path()

from utils.SerialDevices import SerialDevices
from utils.ConfigParser import Commands, ModuleTypes

from db.databaseInterfacer import create_connection

import time
import re
# ! lily was here
# ayo

conn = None
lora_dev = None

def initialize():
    print("Initializing...")

    conn = create_connection()


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
        # serial_devices.read_device(ModuleTypes.LORA_PI)
        lora_serial_input = serial_devices._wait_for_lora_serial_input()

        command = parse_command(lora_serial_input)

        command_type_enum = None
        try:
            command_type_enum = Commands[command]
            # print(command_type_enum)
        except KeyError:
            print(f"No enum member with the name '{command}'")
            continue

        # command = input()
            
        if command_type_enum != Commands.RETRIEVE:
            serial_devices.sendCommandToAllDataModules(command_type_enum.name)
        else:
            for device in serial_devices._serial_devices:

                if (device != ModuleTypes.LORA_PIT) and (device != ModuleTypes.LORA_PI):

                    serial_devices._execute_single_command(Commands.RETRIEVE.name, device)

                    # serial_devices.read_file_data(device)
                    device_serial_obj = serial_devices._serial_devices[device]
                    device_output = "START"
                    while (device_output != "<Finished>"):
                        device_output =  device_serial_obj.readline().decode('utf-8').strip()
                        print(device_output)


if __name__ == "__main__":
    main()