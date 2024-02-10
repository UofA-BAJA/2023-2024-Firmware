import SerialDevices
import time

# ! lily was here

COMMANDS = {'b' : SerialDevices.Commands.BEGIN,
            'e' : SerialDevices.Commands.END,
            'r' : SerialDevices.Commands.RETRIEVE,
            'q' : SerialDevices.Commands.QUIT,
            'help' : SerialDevices.Commands.HELP}

def main():
    serial_devices = SerialDevices.SerialDevices()

    stuff = input()

    serial_devices.execute_command(COMMANDS[stuff])


if __name__ == "__main__":
    main()