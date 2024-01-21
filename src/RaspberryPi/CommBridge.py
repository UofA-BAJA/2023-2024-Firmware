import serial
import serial.tools.list_ports
import time
# https://www.youtube.com/watch?v=jU_b8WBTUew <------ watch this video

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def get_port_paths():
    ports = list(serial.tools.list_ports.comports())
    port_paths = []
    for p in ports:
        print(p.device)
        port_paths.append(p.device)
    return port_paths

def get_serial_devices(port_paths):
    serial_devices = []
    for path in port_paths:
        ser = serial.Serial(path, 9600)
        serial_devices.append(ser)
    time.sleep(3)
    for device in serial_devices:
        device.reset_input_buffer()
    return serial_devices

def begin_logging(serial_devices):
    # send command to nano's to start logging onto their SD cards
    print("Begin logging")


def end_logging(serial_devices):
    # send command to nano's to stop logging onto their SD cards
    print("End logging")

def retrieve_logs(serial_devices):
    # retrieve data from nano's SD cards
    print("Retrieving logs")

def receive_data(serial_devices):
    print("Receiving Data")
    try:
        while True:
            time.sleep(0.05)
            for ser in serial_devices:
                # ser.in_waiting is the num of bytes in the buffer. 
                if ser.in_waiting > 0:
                    data = ser.readline().decode('utf-8').rstrip()
                    print(data)
    except KeyboardInterrupt:
        for ser in serial_devices:
            ser.close()
        print("\nSerial Devices Closed")

def get_input(serial_devices):
    print_commands()
    # Input loop
    commands = {
                'help': lambda : print_commands(),
                'b': lambda : begin_logging(serial_devices),
                'e': lambda : end_logging(serial_devices),
                'r': lambda : retrieve_logs(serial_devices),
                'q': lambda : exit_program(serial_devices)}

    while True:   
        command = str(input())
        # I tried using a match statement here, but I couldn't so this'll have to do.
        if command in commands:
            commands[command]()
        else:
            print(f"{bcolors.WARNING}Warning: Command not found{bcolors.ENDC}")
        time.sleep(.5)

def print_commands():
    print(f"{bcolors.BOLD}", end='')
    print("Commands:")
    print("help -- list of commands")
    print("b -- begin logging")
    print("e -- end logging")
    print("r -- retreive logs")
    print("q -- quit program")
    print(f"{bcolors.ENDC}", end='')

def exit_program(serial_devices):
    for ser in serial_devices:
        ser.close()
        print("\nSerial Devices Closed")
    print("Program exiting")
    exit()


def main():
    port_paths = get_port_paths()
    ser_devices = get_serial_devices(port_paths)

    get_input(ser_devices)
    # receive_data(ser_devices)

if __name__ == "__main__":
    main()