import SerialDevices
import time
from util.ConfigParser import Commands, ModuleTypes
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
    serial_devices = SerialDevices.SerialDevices()

    print("Waiting for devices to connect...")

    # Get the lora device so we know how to send stuff back to the computer.
    lora_dev = serial_devices.get_device(ModuleTypes.LORA_PI)


    while True:
        while lora_dev.in_waiting == 0:
            pass
        command = lora_dev.readline().decode('utf-8').strip()

        command_type_enum = None
        try:
            command_type_enum = Commands[command]
            print(command_type_enum)
        except KeyError:
            print(f"No enum member with the name '{command}'")

        # command = input()
        serial_devices.execute_command(COMMANDS[command_type_enum])


if __name__ == "__main__":
    main()