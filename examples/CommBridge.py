import serial
import serial.tools.list_ports
import time

# https://www.youtube.com/watch?v=jU_b8WBTUew <------ watch this video for basic serial commmunication
#github test
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
        print(f"Device detected: {p.device}")
        port_paths.append(p.device)
    return port_paths

def get_serial_devices(port_paths):
    serial_devices = []
    for path in port_paths:
        ser = serial.Serial(path, 115200)
        serial_devices.append(ser)
    time.sleep(3)
    for device in serial_devices:
        device.reset_input_buffer()
    return serial_devices

def find_LoRa(serial_devices):
    lorapath = None
    return lorapath
    

def begin_logging(serial_devices):
    # send command to nano's to start logging onto their SD cards
    for ser in serial_devices:
        ser.write(b"Begin Logging")

def end_logging(serial_devices):
    # send command to nano's to stop logging onto their SD cards
    for ser in serial_devices:
        ser.write(b"End Logging")

def retrieve_logs(serial_devices):
    files = []
    # retrieve data from nano's SD cards
    for ser in serial_devices:
        # Send command
        ser.write(b"Retrieve Logs")

        # Get rid of all the other crap before.
        ser.flushInput()

        print("Waiting for device response...")
        while(ser.in_waiting == 0):
            pass
        print("Device Responded...")

        # get file name
        file_name = "DEFAULT.TXT"
        file_name = ser.readline().decode('utf-8').rstrip()

        file = open(file_name, 'w')
        # try except so that the file gets closed in case of a forceful program close
        try:
            print("Writing to file...")
            while True:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8')
                    if line.strip() == "Finished":
                        break
                    file.write(line)
            file.close()
        except KeyboardInterrupt:
            file.close()
            exit_program(ser_devices)

        print(f"File {file_name} Created")
        print(f"{bcolors.BOLD}Enter another command{bcolors.ENDC}")


# def receive_data(serial_devices):
#     print("Receiving Data")
#     try:
#         while True:
#             time.sleep(0.05)
#             for ser in serial_devices:
#                 # ser.in_waiting is the num of bytes in the buffer. 
#                 if ser.in_waiting > 0:
#                     data = ser.readline().decode('utf-8').rstrip()
#                     print(data)
#     except KeyboardInterrupt:
#         for ser in serial_devices:
#             ser.close()
#         print("\nSerial Devices Closed")

def get_input(serial_devices, lora):
    print_commands()
    # Input loop
    commands = {
                'help': lambda : print_commands(),
                'b': lambda : begin_logging(serial_devices),
                'e': lambda : end_logging(serial_devices),
                'r': lambda : retrieve_logs(serial_devices),
                'q': lambda : exit_program(serial_devices)}

    while True:   
        command = str(lora.readline().decode('utf-8').rstrip())
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
    lora = find_LoRa(ser_devices)
    get_input(ser_devices,lora)
    # receive_data(ser_devices)

if __name__ == "__main__":
    main()