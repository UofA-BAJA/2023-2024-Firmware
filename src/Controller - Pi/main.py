import SerialDevices
import time
from util.ConfigParser import Commands
# ! lily was here


COMMANDS = {'b' : Commands.BEGIN,
            'e' : Commands.END,
            'r' : Commands.RETRIEVE,
            'q' : Commands.QUIT,
            'help' : Commands.HELP}

def main():
    serial_devices = SerialDevices.SerialDevices()

    while True:
        command = input()
        serial_devices.execute_command(COMMANDS[command])



if __name__ == "__main__":
    main()