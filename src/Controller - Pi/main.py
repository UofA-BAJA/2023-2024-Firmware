from utilsImporter import adding_utils_to_path

adding_utils_to_path()

from utils.SerialDevices import SerialDevices
from utils.ConfigParser import Commands, ModuleTypes

from db.databaseInterfacer import get_BajaCloud_connection, insert_session

import time
import re
# ! lily was here
# ayo



class Controller:

    def __init__(self):
        self.serial_devices = SerialDevices()
        self.conn = get_BajaCloud_connection()

    def handleCommand(self, command_type_enum):

        if command_type_enum != Commands.RETRIEVE:
            self.serial_devices.sendCommandToAllDataModules(command_type_enum.name)
        else:
            for device in self.serial_devices._serial_devices:

                if (device != ModuleTypes.LORA_PIT) and (device != ModuleTypes.LORA_PI):

                    self.serial_devices._execute_single_command(Commands.RETRIEVE.name, device)

                    # serial_devices.read_file_data(device)
                    device_serial_obj = self.serial_devices._serial_devices[device]
                    device_output = "START"
                    while (device_output != "<Finished>"):
                        device_output =  device_serial_obj.readline().decode('utf-8').strip()
                        print(device_output)

    def run(self):
        if (ModuleTypes.LORA_PIT in self.serial_devices._serial_devices):
            self.serial_devices._serial_devices[ModuleTypes.LORA_PIT].close()

        lora_device = self.serial_devices.get_device(ModuleTypes.LORA_PI)

        if (lora_device) == None:
            print("No LORA PI DEVICE FOUND!!!!!!")
            while(1): pass

        while True:
            lora_serial_input = self.serial_devices._wait_for_lora_serial_input()

            try:
                parsed_input = Controller.parse_input(lora_serial_input)
                command_type_enum = Commands[parsed_input]
                self.handleCommand(command_type_enum=command_type_enum)
            except KeyError:
                print(f"Serial input: {lora_serial_input} is not a valid command.")

            if "SESSION" in lora_serial_input:
                actual_text = Controller.parse_input(lora_serial_input).replace("SESSION:", "")

                insert_session(self.conn, actual_text)

                print(f"ADDING SESSION: '{actual_text}' INTO DATABASE")
                break
        
        self.conn.close()


    def parse_input(raw):
        # print(f"idk {command}")
        pattern = re.compile(r'{(.*?)}')
        # Find all matches in the text
        matches = pattern.findall(raw)

        return matches[0]

        


if __name__ == "__main__":
    c = Controller()

    c.run()