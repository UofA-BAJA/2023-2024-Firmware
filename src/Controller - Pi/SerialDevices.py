import serial
import serial.tools.list_ports
from util.ConfigParser import Commands, ModuleTypes 
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
        serial_devices = {}
        temp_devices = []
        for path in self._port_paths:
            ser = serial.Serial(path, 115200)
            temp_devices.append(ser)

        time.sleep(2)

        for ser in temp_devices:
            ser.flushInput()
            ser.flushOutput()
            time.sleep(.3)
            ser.write(Commands.SEND_TYPE.name.encode('utf-8'))
            while(ser.in_waiting == 0):
                pass

            dev_type = ser.readline().decode('utf-8').strip()
            print(dev_type)
            dev_type_enum = None
            try:
                dev_type_enum = ModuleTypes[dev_type]
                print(dev_type_enum)
            except KeyError:
                print(f"No enum member with the name '{dev_type}'")

            ser.flushInput()
            serial_devices[dev_type_enum] = ser
            
        return serial_devices

    def get_device(self, dev_type):
        if not dev_type in self._serial_devices:
            print(f"No device with type {dev_type} found.")
            return
        else:
            self._serial_devices[dev_type]

    def execute_command(self, command, dev_type = None):
        """ 
            Executes the commands <---- very descriptive, I know =D
        """

        # If no parameter is passed for dev_type, that means the command is sent to every device.
        if dev_type == None:
            commands = {
                    Commands.HELP     : lambda : self._print_commands(),
                    Commands.BEGIN    : lambda : self._begin_logging(),
                    Commands.END      : lambda : self._end_logging(),
                    Commands.RETRIEVE : lambda : self._retrieve_logs(),
                    Commands.QUIT     : lambda : self._quit_program()}
                
            commands[command]()
        else:
            commands = {
                    Commands.HELP     : lambda : self._print_commands(),
                    Commands.BEGIN    : lambda : self._begin_logging(dev_type),
                    Commands.END      : lambda : self._end_logging(dev_type),
                    Commands.RETRIEVE : lambda : self._retrieve_logs(dev_type),
                    Commands.QUIT     : lambda : self._quit_program()}
                
            commands[command]()




# TODO: UPDATE THE SEND FUNCTIONS TO WORK USING THE ENUMS

    def _begin_logging(self, dev_type = None):
        print(f"Command Sent : -- {bcolors.OKBLUE}Begin Logging{bcolors.ENDC} --")

        # If no parameter is passed for dev_type, that means the command is sent to every device.
        if dev_type == None:
            # send command to nano's to start logging onto their SD cards
            for dev in self._serial_devices:
                self._serial_devices[dev].write(b"Begin Logging")
        else:
            self._serial_devices[dev_type].write(b"Begin Logging")


    def _end_logging(self, dev_type = None):
        print(f"Command Sent : -- {bcolors.OKBLUE}End Logging{bcolors.ENDC} --")
        # send command to nano's to stop logging onto their SD cards

        # If no parameter is passed for dev_type, that means the command is sent to every device.
        if dev_type == None:
            for dev in self._serial_devices:
                self._serial_devices[dev].write(b"End Logging")
        else:
            self._serial_devices[dev_type].write(b"End Logging")

    
    def _retrieve_logs(self,  dev_type = None):
        print(f"Command Sent : -- {bcolors.OKBLUE}Retrieve Logs{bcolors.ENDC} --")

        # retrieve data from nano's SD cards

        # If no parameter is passed for dev_type, that means the command is sent to every device.
        if dev_type == None:
            for dev in self._serial_devices:
                self._retrieve_dev_log(dev)
        else:
            self._retrieve_dev_log(dev_type)
    
    def _retrieve_dev_log(self, dev_type):
            # Send command
            self._serial_devices[dev_type].write(b"Retrieve Logs")

            # Get rid of all the other crap before.
            self._serial_devices[dev_type].flushInput()

            print("Waiting for device response...")
            while(self._serial_devices[dev_type].in_waiting == 0):
                pass
            print("Device Responded...")

            # get file name
            file_name = "DEFAULT.TXT"
            file_name = self._serial_devices[dev_type].readline().decode('utf-8').rstrip()

            file = open(file_name, 'w')
            # try except so that the file gets closed in case of a forceful program close
            try:
                print("Writing to file...")
                while True:
                    if self._serial_devices[dev_type].in_waiting > 0:
                        line = self._serial_devices[dev_type].readline().decode('utf-8')
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