from utilsImporter import adding_utils_to_path

adding_utils_to_path()

from utils.ConfigParser import Commands, ModuleTypes
from utils.SerialDevices import SerialDevices
'''
plan
- get LORA_PIT serial device
- send command to LORA_PIT
- wait for response
- print response
- retrieve logs
'''

class CactusControlCLI:
    def __init__(self):

        self.lora_device = None

        self.commands = {
            Commands.HELP.name     : self._print_commands,  # No argument required; directly reference the method
            Commands.BEGIN.name    : lambda: self._begin_logging(Commands.BEGIN.name),  # Use lambda for delayed execution with arguments
            Commands.END.name      : lambda: self._end_logging(Commands.END.name),  # Ditto
            Commands.RETRIEVE.name : lambda: self._retrieve_logs(Commands.RETRIEVE.name),  # Ditto
            Commands.QUIT.name     : lambda: self._quit_program(Commands.QUIT.name),  # Ditto
        }

    def _print_commands(self):
        for command, _ in self.commands.items():
            print(f"{getattr(bcolors, 'OKCYAN')}{command}{getattr(bcolors, 'ENDC')}")

    def display_commands_with_colors(self):
        print(f"{bcolors.HEADER}Available commands:{bcolors.ENDC}")
        self._print_commands()

    def _write_to_lora_device(self, command):
        self.lora_device.write(f"<{command}>".encode('utf-8'))

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

    
    
    def run(self):
        serial_devices = SerialDevices()

        for device in serial_devices._serial_devices:

            if (device != ModuleTypes.LORA_PIT):
                serial_devices.close_serial_port(device)
                
        self.lora_device = serial_devices.get_device(ModuleTypes.LORA_PIT)

        if (self.lora_device) == None:

            print("No LORA PIT DEVICE FOUND!!!!!!")
            while(1): pass

        while True:
            self.display_commands_with_colors()
            choice = input("Please choose a command: ").upper()

            action = self.commands.get(choice)

            if action:
                action()
                self._write_to_lora_device(choice)

                serial_devices.read_device(ModuleTypes.LORA_PIT)


            else:
                print(f"{bcolors.FAIL}Invalid command. Please try again.{bcolors.ENDC}")

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


