import serial
import serial.tools.list_ports
from enum import Enum
import time
import sys

class SerialDevices:
    def __init__(self):
        ports = list(serial.tools.list_ports.comports())
        port_paths = []
        for p in ports:
            print(f"Device detected: {p.device}")
            port_paths.append(p.device)
        self._port_paths = port_paths
        self._serial_devices = self.get_serial_devices()

    def get_serial_devices(self):
        """ 
            Gets the serial devices hahahahahahah
        """
        serial_devices = []
        for path in self._port_paths:
            ser = serial.Serial(path, 115200)
            serial_devices.append((ser, None))

        time.sleep(2)
        for ser in serial_devices:
            # ser[0].write(b"Send Type")
            
            # while(ser[0].in_waiting == 0):
            #     pass
            # dev_type = ser[0].readline()
            # ser[1] = dev_type
            ser[0].reset_input_buffer()
        return serial_devices

    def execute_command(self, command):
        """ 
            Executes the commands hahahahahahaha
        """
        commands = {
                Commands.HELP     : lambda : self._print_commands(),
                Commands.BEGIN    : lambda : self._begin_logging(),
                Commands.END      : lambda : self._end_logging(),
                Commands.RETRIEVE : lambda : self._retrieve_logs(),
                Commands.QUIT     : lambda : self._quit_program()}
            
        commands[command]()


    def _begin_logging(self):
        print(f"Command Sent : -- {bcolors.OKBLUE}Begin Logging{bcolors.ENDC} --")
        # send command to nano's to start logging onto their SD cards
        for ser in self._serial_devices:
            ser[0].write(b"Begin Logging")

    def _end_logging(self):
        print(f"Command Sent : -- {bcolors.OKBLUE}End Logging{bcolors.ENDC} --")
        # send command to nano's to stop logging onto their SD cards
        for ser in self._serial_devices:
            ser[0].write(b"End Logging")
    
    def _retrieve_logs(self):
        print(f"Command Sent : -- {bcolors.OKBLUE}Retrieve Logs{bcolors.ENDC} --")

        files = []
        # retrieve data from nano's SD cards
        for ser in self._serial_devices:
            # Send command
            ser[0].write(b"Retrieve Logs")

            # Get rid of all the other crap before.
            ser[0].flushInput()

            print("Waiting for device response...")
            while(ser[0].in_waiting == 0):
                pass
            print("Device Responded...")

            # get file name
            file_name = "DEFAULT.TXT"
            file_name = ser[0].readline().decode('utf-8').rstrip()

            file = open(file_name, 'w')
            # try except so that the file gets closed in case of a forceful program close
            try:
                print("Writing to file...")
                while True:
                    if ser[0].in_waiting > 0:
                        line = ser[0].readline().decode('utf-8')
                        if line.strip() == "Finished":
                            break
                        file.write(line)
                file.close()
            except KeyboardInterrupt:
                file.close()
                self._quit_program()

            print(f"File {file_name} Created")
            print(f"{bcolors.BOLD}Enter another command{bcolors.ENDC}")
    
    def _print_commands(self):
        print(f"{bcolors.OKBLUE}Command List{bcolors.ENDC}")

        # print some commands or something here
        pass


    # * idk if we need this but whatever. This will send a command to the pi to quit or something...?
    def _quit_program(self):
        sys.exit(1)


class Commands(Enum):
    BEGIN = 0
    END = 1
    RETRIEVE = 2
    HELP = 3
    QUIT = 4

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