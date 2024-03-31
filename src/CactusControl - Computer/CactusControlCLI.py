import re

from UofA_BAJA_2023_2024_common.enums import Commands, ModuleTypes, WirelessNodeTypes, MessageHeaders
from UofA_BAJA_2023_2024_common.SerialDevices import SerialDevices
from UofA_BAJA_2023_2024_common.Messages import construct_message



class CactusControlCLI:
    def __init__(self):

        self.lora_device = None
        self.session_is_active = False

        self.commands = {
            Commands.HELP     : self._print_commands,  # No argument required; directly reference the method
            Commands.BEGIN    : lambda: self._begin_logging(Commands.BEGIN),  # Use lambda for delayed execution with arguments
            Commands.END      : lambda: self._end_logging(Commands.END),  # Ditto
            Commands.RETRIEVE : lambda: self._retrieve_logs(Commands.RETRIEVE),  # Ditto
            Commands.QUIT    : lambda: self._quit_program(Commands.QUIT),  # Ditto
            "SETUP"    : lambda: self._setup_session(), # Ditto
            "READ"     : lambda: self._read_data() # Ditto
        }

    def _print_commands(self):
        for command, _ in self.commands.items():
            print(f"{getattr(bcolors, 'OKCYAN')}{command}{getattr(bcolors, 'ENDC')}")

    def display_commands_with_colors(self):
        print(f"{bcolors.HEADER}Available commands:{bcolors.ENDC}")
        self._print_commands()

    def send_command_to_rasberry_pi(self, command):
        message = construct_message(WirelessNodeTypes.RASBERRYPI, command)
        self.lora_device.write(f"<{message}>".encode('utf-8'))

    def _write_to_lora_device(self, command):
        self.lora_device.write(f"<{command}>".encode('utf-8'))

    def wait_for_response(self):
        response = ""

        while True:
            response = self.serial_devices.does_device_have_bracketed_output(ModuleTypes.LORA_PIT)

            if response != "":
                # print(f"{bcolors.OKGREEN}From pi:{response}{bcolors.ENDC}")
                self.parse_response_for_mesg(response)
                break

    def parse_response_for_mesg(self, response):
        regex_pattern_logic = r"(.*?)"

        regex = f"{MessageHeaders.BODY}{regex_pattern_logic}{MessageHeaders.END}"

        match = re.search(regex, response)
        extracted_str = match.group(1) if match else "No match found"
        
        print(f"{bcolors.OKGREEN}Rasberry Pi:\n{extracted_str}{bcolors.ENDC}\n")

    def _begin_logging(self, command):
        
        print("Beginning logging...")        

    def _end_logging(self, command):
        print("Ending logging...")
        # Implement your end logging logic here

        print("Logging has ended.")


    def _retrieve_logs(self, command):
        print("Retrieving logs...")
        # Implement your retrieve logs logic here


    def _quit_program(self, command):
        print("Quitting program...")
        exit()

    def _setup_session(self):
        '''code to setup session'''
        print(f"{bcolors.HEADER}Setup New Data Session{bcolors.ENDC}")
        
        # Ask the user for session name and notes
        session_name = input(f"{bcolors.OKCYAN}Enter session name and/or notes: {bcolors.ENDC}")
        
        # Send the setup command over serial
        self.send_command_to_rasberry_pi(f"SESSION:{session_name}")
        
        # Confirmation message
        print(f"{bcolors.OKGREEN}Session '{session_name}'{bcolors.ENDC}")

        self.wait_for_response()

        self.session_is_active = True

    def _read_data(self):
        '''code to read data'''
        
        # Send the read command over serial
        self.serial_devices.read_device(ModuleTypes.LORA_PIT)
        

    def run(self):
        self.serial_devices = SerialDevices()

        for device in self.serial_devices._serial_devices:

            if (device != ModuleTypes.LORA_PIT):
                self.serial_devices.close_serial_port(device)
                
        self.lora_device = self.serial_devices.get_device(ModuleTypes.LORA_PIT)

        if (self.lora_device) == None:

            print("No LORA PIT DEVICE FOUND!!!!!!")
            while(1): pass

        while True:
            self.display_commands_with_colors()
            choice = input("Please choose a command: ").upper()

            action = self.commands.get(choice)

            if action:
                action()

                self.send_command_to_rasberry_pi(choice)

                if (CactusControlCLI.is_command(choice)):
                    self.wait_for_response()


            else:
                print(f"{bcolors.FAIL}Invalid command. Please try again.{bcolors.ENDC}")

    def is_command(command_str):
        commands = {attribute: value for attribute, value in Commands.__dict__.items() if not attribute.startswith('__')}
        return command_str in commands.values()

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

if __name__ == "__main__":
    app = CactusControlCLI()
    app.run()


