
from util.ConfigParser import Commands, ModuleTypes 
from SerialHandling import get_lora_pit_serial_device, send_string_over_serial
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

    def _begin_logging(self, command):
        print("Beginning logging...")
        
        send_string_over_serial(command)

    def _end_logging(self, command):
        print("Ending logging...")
        # Implement your end logging logic here
        send_string_over_serial(command)


    def _retrieve_logs(self, command):
        print("Retrieving logs...")
        # Implement your retrieve logs logic here
        send_string_over_serial(command)


    def _quit_program(self, command):
        print("Quitting program...")
        exit()

    
    
    def run(self):
        while True:
            get_lora_pit_serial_device()
            self.display_commands_with_colors()
            choice = input("Please choose a command: ").upper()
            action = self.commands.get(choice)

            if action:
                action()
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


