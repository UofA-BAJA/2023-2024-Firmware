import serial
import serial.tools.list_ports
from serial.serialutil import SerialException

from enum import Enum
import time
import sys

from ConfigParser import Commands, ModuleTypes 


class SerialDevices:
    def __init__(self):
        ports = list(serial.tools.list_ports.comports())
        port_paths = []
        for p in ports:
            print(p.description)
            # Example of filtering out Bluetooth devices by checking for a keyword in the description
            # Adjust the keyword according to your needs
            if 'Bluetooth' not in p.description and "n/a" not in p.description:
                print(f"Device detected: {p.device}")
                port_paths.append(p.device)
            else:
                print(f"Bluetooth device ignored: {p.device}")

        self._port_paths = port_paths
        self._serial_devices = self.get_all_serial_devices()

        print(f"Found devices...\n")
        for dev in self._serial_devices:
            print(f"{dev.name} on {self._serial_devices[dev].port}")

    def get_all_serial_devices(self):
        """ 
            Gets the serial devices hahahahahahah
        """
        serial_devices = {}

        time.sleep(1)

        for path in self._port_paths:
            ser = None
            try:
                ser = serial.Serial(path, 115200, timeout=5)
            except SerialException as e:
                # Print the exception details
                print(f"Failed to open serial port {path}: {e}")
                continue
            
            ser.setDTR(False) # Drop DTR
            time.sleep(0.023)    # Read somewhere that 22ms is what the UI does.
            ser.setDTR(True)  # UP the DTR back

            ser.flushInput()
            ser.flushOutput()
            time.sleep(.3)

            is_device_ready = False
            print(f"\nDevice Output on {ser.port}:")
            while not is_device_ready:
                try:
                    device_output =  ser.readline().decode('utf-8').strip()
                except UnicodeDecodeError:
                    print("UnicodeDecodeError")
                    break
                print(device_output)
                if device_output == "Ready":
                    is_device_ready = True
            print(f"End of Device Output on {ser.port}\n")

                    
            ser.write(f"<{Commands.SENDTYPE.name}>".encode('utf-8'))
            
            dev_type = None
            while ser.in_waiting == 0:
                dev_type = ser.readline().decode('utf-8').strip()
                break
            
            dev_type_enum = None
            try:
                dev_type_enum = ModuleTypes[dev_type]
                print(dev_type_enum)
            except KeyError:
                print(f"No enum member with the name '{dev_type}'")

            ser.flushInput()
            serial_devices[dev_type_enum] = ser


        return serial_devices

    def close_serial_port(self, device_type):

        self._serial_devices[device_type].close()

    def get_device(self, dev_type):
        if not dev_type in self._serial_devices:
            print(f"No device with type {dev_type} found.")
            return None
        else:
            return self._serial_devices[dev_type]

    def _execute_single_command(self, command, devtype):
        """ 
            Executes the commands <---- very descriptive, I know =D
        """
        serial_connection = self._serial_devices[devtype]

        print(f"Sending Command to {bcolors.GRAYCOLOR} {devtype} {bcolors.GRAYCOLOR}: -- {bcolors.OKBLUE} {command} {bcolors.ENDC} --")
        serial_connection.write(f"<{command}>".encode('utf-8'))
        


    # TODO: UPDATE THE SEND FUNCTIONS TO WORK USING THE ENUMS

    def _wait_for_lora_serial_input(self):
        lora_device = self.get_device(ModuleTypes.LORA_PI)

        commandInput = False
        device_output = ""
        while not commandInput:
            device_output =  lora_device.readline().decode('utf-8').strip()                
            # print(f"this? {device_output}")
            # print(type(device_output))
                
            
            if "<" in device_output and ">" in device_output:
                commandInput = True
                break

            if len(device_output) > 0:
                print(f"No command but Serial Input: {device_output}")
        
        return device_output
    
    def sendCommandToAllDataModules(self, command):

        for dev in self._serial_devices:
            
            if (dev != ModuleTypes.LORA_PI) and (dev != ModuleTypes.LORA_PIT):
                
                self._execute_single_command(command, dev)

    def read_device(self, device_type):
        device = self.get_device(device_type)
        
        device_output = "START"
        first_time_print_flag = True
        while (device_output != ""):
            device_output =  device.readline().decode('utf-8').strip()

            if "<" in device_output and ">" in device_output:
                break

            if (device_output != ""):

                if (first_time_print_flag):
                    print(f"{bcolors.GRAYCOLOR}{bcolors.UNDERLINE}\nOUTPUT FOR {device_type.name}:{bcolors.ENDC}\n")

                    device_has_output_flag = False

                print(f"{bcolors.GRAYCOLOR}{device_output}{bcolors.ENDC}")


    def read_file_data(self, device_type):
        device = self.get_device(device_type)

        print(f"{bcolors.GRAYCOLOR}{bcolors.UNDERLINE}\nFILE DATA FOR {device_type.name}:{bcolors.ENDC}\n")

        device_output = "START"

        while (device_output != "<Finished>"):
            device_output =  device.readline().decode('utf-8').strip()


    # def _begin_logging(self, dev_type = None):

    #     # If no parameter is passed for dev_type, that means the command is sent to every device.
    #     if dev_type == None:
    #         # send command to nano's to start logging onto their SD cards
    #         for dev in self._serial_devices:

    #             if (dev != ModuleTypes.LORA_PI):
    #                 self._serial_devices[dev].write(b"<BEGIN>")
    #     else:
    #         self._serial_devices[dev_type].write(b"Begin Logging")


    # def _end_logging(self, dev_type = None):
    #     print(f"Command Sent : -- {bcolors.OKBLUE}End Logging{bcolors.ENDC} --")
    #     # send command to nano's to stop logging onto their SD cards

    #     # If no parameter is passed for dev_type, that means the command is sent to every device.
    #     if dev_type == None:
    #         for dev in self._serial_devices:
    #             self._serial_devices[dev].write(b"End Logging")
    #     else:
    #         self._serial_devices[dev_type].write(b"End Logging")

    
    # def _retrieve_logs(self,  dev_type = None):
    #     print(f"Command Sent : -- {bcolors.OKBLUE}Retrieve Logs{bcolors.ENDC} --")

    #     # retrieve data from nano's SD cards

    #     # If no parameter is passed for dev_type, that means the command is sent to every device.
    #     if dev_type == None:
    #         for dev in self._serial_devices:
    #             self._retrieve_dev_log(dev)
    #     else:
    #         self._retrieve_dev_log(dev_type)
    
    # def _retrieve_dev_log(self, dev_type):
    #     # Send command
    #     self._serial_devices[dev_type].write(b"Retrieve Logs")

    #     # Get rid of all the other crap before.
    #     self._serial_devices[dev_type].flushInput()

    #     print("Waiting for device response...")
    #     while(self._serial_devices[dev_type].in_waiting == 0):
    #         pass
    #     print("Device Responded...")

    #     # get file name
    #     file_name = "DEFAULT.TXT"
    #     file_name = self._serial_devices[dev_type].readline().decode('utf-8').rstrip()

    #     file = open(file_name, 'w')
    #     # try except so that the file gets closed in case of a forceful program close
    #     try:
    #         print("Writing to file...")
    #         while True:
    #             if self._serial_devices[dev_type].in_waiting > 0:
    #                 line = self._serial_devices[dev_type].readline().decode('utf-8')
    #                 if line.strip() == "Finished":
    #                     break
    #                 file.write(line)
    #         file.close()
    #     except KeyboardInterrupt:
    #         file.close()
    #         self._quit_program()

    #     print(f"File {file_name} Created")
    #     print(f"{bcolors.BOLD}Enter another command{bcolors.ENDC}")


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
    GRAYCOLOR = "\033[38;5;240m"