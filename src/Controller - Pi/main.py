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


    print(f"Found devices...\n")
    for dev in serial_devices._serial_devices:
        print(f"{dev.name} on {serial_devices._serial_devices[dev].port}")


    serial_devices._send_command("Hello from the Pi")
    while True:
        command = serial_devices._wait_for_lora_serial_input()
        
        command = command.replace(">", "").replace("<", "").strip()
        print(f"Command received: {command}")

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