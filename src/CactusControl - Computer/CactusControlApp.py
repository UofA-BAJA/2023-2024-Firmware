import re
import json
import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QMessageBox, QInputDialog
from PyQt6.QtCore import Qt
from UofA_BAJA_2023_2024_common.enums import Commands, ModuleTypes, WirelessNodeTypes, MessageHeaders
from UofA_BAJA_2023_2024_common.SerialDevices import SerialDevices
from UofA_BAJA_2023_2024_common.Messages import construct_message

class CactusControlApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.lora_device = None
        self.session_is_active = False
        self.responses = []
        self.setup_ui()

    def setup_ui(self):
        from CactusControlApp import Ui_MainWindow
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.pushButtonHelp.clicked.connect(lambda: self.execute_command("HELP"))
        self.ui.pushButtonBegin.clicked.connect(lambda: self.execute_command("BEGIN"))
        self.ui.pushButtonEnd.clicked.connect(lambda: self.execute_command("END"))
        self.ui.pushButtonRetrieve.clicked.connect(lambda: self.execute_command("RETRIEVE"))
        self.ui.pushButtonQuit.clicked.connect(lambda: self.execute_command("QUIT"))
        self.ui.pushButtonSetup.clicked.connect(lambda: self.execute_command("SETUP"))
        self.ui.pushButtonRead.clicked.connect(lambda: self.execute_command("READ"))

    def execute_command(self, command):
        action = self.get_command_action(command)
        if action:
            action()
            self.send_command_to_rasberry_pi(command)
            
            if self.is_command(command):
                self.wait_for_response()
        else:
            self.show_error("Invalid command. Please try again.")

    def get_command_action(self, command):
        commands = {
            "HELP": self.print_commands,
            "BEGIN": self.begin_logging,
            "END": self.end_logging,
            "RETRIEVE": self.retrieve_logs,
            "QUIT": self.quit_program,
            "SETUP": self.setup_session,
            "READ": self.read_data
        }
        return commands.get(command)

    def print_commands(self):
        for command, _ in self.commands.items():
            print(f"{self.get_color('OKCYAN')}{command}{self.get_color('ENDC')}")

    def send_command_to_rasberry_pi(self, command):
        message = construct_message(WirelessNodeTypes.RASBERRYPI, command)
        self.lora_device.write(f"<{message}>".encode('utf-8'))

    def wait_for_response(self):
        self.responses = []
        while True:
            try:
                important_serial_data = self.serial_devices.does_device_have_bracketed_output(ModuleTypes.LORA_PIT)
                if important_serial_data != "":
                    parsed_response = self.parse_response_for_mesg(important_serial_data)
                    self.responses.append(parsed_response)
                    
                    if "DONE-WITH-MSG" in parsed_response:
                        break
            except KeyboardInterrupt:
                break
        
        self.parse_responses()

    def parse_response_for_mesg(self, response):
        regex_pattern_logic = r"(.*?)"
        regex = f"{MessageHeaders.BODY}{regex_pattern_logic}{MessageHeaders.END}"
        match = re.search(regex, response)
        extracted_str = match.group(1) if match else "No match found"
        
        return extracted_str

    def parse_responses(self):
        for response in self.responses:
            try:
                json_response = json.loads(response)
                if "datatypes" in json_response:
                    self.have_user_select_data_types(json_response)
                if "data-packet" in json_response:
                    self.show_info(f"Data Packet Received: {json_response['data-packet']}")
            except json.JSONDecodeError:
                self.show_error("Could not decode JSON response")
                continue

    def have_user_select_data_types(self, datatypes: dict):
        user_input, ok = QInputDialog.getText(self, "Input", "Please enter a data type from the list above:")
        if ok and user_input in datatypes['datatypes']:
            self.send_command_to_rasberry_pi(json.dumps({"data-query" : {"selected-datatype": user_input}}))
            self.wait_for_response()
        else:
            self.show_error("Invalid data type. Please try again.")

    def begin_logging(self):
        print("Beginning logging...")

    def end_logging(self):
        print("Logging has ended.")

    def retrieve_logs(self):
        print("Retrieving logs...")

    def quit_program(self):
        print("Quitting program...")
        sys.exit()

    def setup_session(self):
        session_name, ok = QInputDialog.getText(self, "Input", "Enter session name and/or notes:")
        if ok:
            self.send_command_to_rasberry_pi(f"SESSION:{session_name}")
            print(f"Session '{session_name}'")
            self.wait_for_response()
            self.session_is_active = True

    def read_data(self):
        self.serial_devices.read_device(ModuleTypes.LORA_PIT)

    def get_color(self, color):
        colors = {
            "HEADER": '\033[95m',
            "OKBLUE": '\033[94m',
            "OKCYAN": '\033[96m',
            "OKGREEN": '\033[92m',
            "WARNING": '\033[93m',
            "FAIL": '\033[91m',
            "ENDC": '\033[0m',
            "BOLD": '\033[1m',
            "UNDERLINE": '\033[4m'
        }
        return colors.get(color)

    @staticmethod
    def is_command(command_str):
        commands = {attribute: value for attribute, value in Commands.__dict__.items() if not attribute.startswith('__')}
        return command_str in commands.values()

    def show_error(self, message):
        QMessageBox.critical(self, "Error", message)

    def show_info(self, message):
        QMessageBox.information(self, "Information", message)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = CactusControlApp()
    window.show()
    sys