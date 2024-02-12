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

    while True:
        command = input()
        serial_devices.execute_command(COMMANDS[command])



if __name__ == "__main__":
    main()