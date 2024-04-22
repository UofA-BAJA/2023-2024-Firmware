# import re
# import json
# import sys
# from PyQt6.QtWidgets import QApplication, QMainWindow, QMessageBox, QInputDialog, QPushButton, QWidget
# from UofA_BAJA_2023_2024_common.enums import Commands, ModuleTypes, WirelessNodeTypes, MessageHeaders
# from UofA_BAJA_2023_2024_common.SerialDevices import SerialDevices
# from UofA_BAJA_2023_2024_common.Messages import construct_message

# # Generated GUI code
# class Ui_MainWindow(object):
#     def setupUi(self, MainWindow):
#         MainWindow.setObjectName("MainWindow")
#         MainWindow.resize(800, 600)
#         self.centralwidget = QtWidgets.QWidget(parent=MainWindow)
#         self.centralwidget.setObjectName("centralwidget")
#         self.pushButton = QtWidgets.QPushButton(parent=self.centralwidget)
#         self.pushButton.setGeometry(QtCore.QRect(20, 230, 75, 24))
#         self.pushButton.setObjectName("pushButton")
#         self.pushButton_2 = QtWidgets.QPushButton(parent=self.centralwidget)
#         self.pushButton_2.setGeometry(QtCore.QRect(120, 230, 75, 24))
#         self.pushButton_2.setObjectName("pushButton_2")
#         self.pushButton_3 = QtWidgets.QPushButton(parent=self.centralwidget)
#         self.pushButton_3.setGeometry(QtCore.QRect(220, 230, 75, 24))
#         self.pushButton_3.setObjectName("pushButton_3")
#         self.pushButton_4 = QtWidgets.QPushButton(parent=self.centralwidget)
#         self.pushButton_4.setGeometry(QtCore.QRect(320, 230, 75, 24))
#         self.pushButton_4.setObjectName("pushButton_4")
#         self.pushButton_5 = QtWidgets.QPushButton(parent=self.centralwidget)
#         self.pushButton_5.setGeometry(QtCore.QRect(410, 230, 75, 24))
#         self.pushButton_5.setObjectName("pushButton_5")
#         self.pushButton_6 = QtWidgets.QPushButton(parent=self.centralwidget)
#         self.pushButton_6.setGeometry(QtCore.QRect(500, 230, 75, 24))
#         self.pushButton_6.setObjectName("pushButton_6")
#         self.pushButton_7 = QtWidgets.QPushButton(parent=self.centralwidget)
#         self.pushButton_7.setGeometry(QtCore.QRect(590, 230, 75, 24))
#         self.pushButton_7.setObjectName("pushButton_7")
#         MainWindow.setCentralWidget(self.centralwidget)
#         self.retranslateUi(MainWindow)
#         QtCore.QMetaObject.connectSlotsByName(MainWindow)

#     def retranslateUi(self, MainWindow):
#         _translate = QtCore.QCoreApplication.translate
#         MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
#         self.pushButton.setText(_translate("MainWindow", "HELP"))
#         self.pushButton_2.setText(_translate("MainWindow", "BEGIN"))
#         self.pushButton_3.setText(_translate("MainWindow", "END"))
#         self.pushButton_4.setText(_translate("MainWindow", "RETRIEVE"))
#         self.pushButton_5.setText(_translate("MainWindow", "QUIT"))
#         self.pushButton_6.setText(_translate("MainWindow", "SETUP"))
#         self.pushButton_7.setText(_translate("MainWindow", "READ"))

# # Main application class
# class CactusControlApp(QMainWindow, Ui_MainWindow):
#     def __init__(self):
#         super().__init__()
#         self.lora_device = None
#         self.session_is_active = False
#         self.responses = []
#         self.setup_ui()

#     def setup_ui(self):
#         self.setupUi(self)
#         self.pushButton.clicked.connect(lambda: self.execute_command("HELP"))
#         self.pushButton_2.clicked.connect(lambda: self.execute_command("BEGIN"))
#         self.pushButton_3.clicked.connect(lambda: self.execute_command("END"))
#         self.pushButton_4.clicked.connect(lambda: self.execute_command("RETRIEVE"))
#         self.pushButton_5.clicked.connect(lambda: self.execute_command("QUIT"))
#         self.pushButton_6.clicked.connect(lambda: self.execute_command("SETUP"))
#         self.pushButton_7.clicked.connect(lambda: self.execute_command("READ"))

#     def execute_command(self, command):
#         action = self.get_command_action(command)
#         if action:
#             action()
#             self.send_command_to_rasberry_pi(command)
            
#             if self.is_command(command):
#                 self.wait_for_response()
#         else:
#             self.show_error("Invalid command. Please try again.")

#     def get_command_action(self, command):
#         commands = {
#             "HELP": self.print_commands,
#             "BEGIN": self.begin_logging,
#             "END": self.end_logging,
#             "RETRIEVE": self.retrieve_logs,
#             "QUIT": self.quit_program,
#             "SETUP": self.setup_session,
#             "READ": self.read_data
#         }
#         return commands.get(command)

#     def print_commands(self):
#         for command, _ in self.commands.items():
#             print(f"{self.get_color('OKCYAN')}{command}{self.get_color('ENDC')}")

#     def send_command_to_rasberry_pi(self, command):
#         message = construct_message(WirelessNodeTypes.RASBERRYPI, command)
#         self.lora_device.write(f"<{message}>".encode('utf-8'))

#     def wait_for_response(self):
#         self.responses = []
#         while True:
#             try:
#                 important_serial_data = self.serial_devices.does_device_have_bracketed_output(ModuleTypes.LORA_PIT)
#                 if important_serial_data != "":
#                     parsed_response = self.parse_response_for_mesg(important_serial_data)
#                     self.responses.append(parsed_response)
                    
#                     if "DONE-WITH-MSG" in parsed_response:
#                         break
#             except KeyboardInterrupt:
#                 break
        
#         self.parse_responses()

#     def parse_response_for_mesg(self, response):
#         regex_pattern_logic = r"(.*?)"
#         regex = f"{MessageHeaders.BODY}{regex_pattern_logic}{MessageHeaders.END}"
#         match = re.search(regex, response)
#         extracted_str = match.group(1) if match else "No match found"
        
#         return extracted_str

#     def parse_responses(self):
#         for response in self.responses:
#             try:
#                 json_response = json.loads(response)
#                 if "datatypes" in json_response:
#                     self.have_user_select_data_types(json_response)
#                 if "data-packet" in json_response:
#                     self.show_info(f"Data Packet Received: {json_response['data-packet']}")
#             except json.JSONDecodeError:
#                 self.show_error("Could not decode JSON response")
#                 continue

#     def have_user_select_data_types(self, datatypes: dict):
#         user_input, ok = QInputDialog.getText(self, "Input", "Please enter a data type from the list above:")
#         if ok and user_input in datatypes['datatypes']:
#             self.send_command_to_rasberry_pi(json.dumps({"data-query" : {"selected-datatype": user_input}}))
#             self.wait_for_response()
#         else:
#             self.show_error("Invalid data type. Please try again.")

#     def begin_logging(self):
#         print("Beginning logging...")

#     def end_logging(self):
#         print("Logging has ended.")

#     def retrieve_logs(self):
#         print("Retrieving logs...")

#     def quit_program(self):
#         print("Quitting program...")
#         sys.exit()

#     def setup_session(self):
#         session_name, ok = QInputDialog.getText(self, "Input", "Enter session name and/or notes:")
#         if ok:
#             self.send_command_to_rasberry_pi(f"SESSION:{session_name}")
#             print(f"Session '{session_name}'")
#             self.wait_for_response()
#             self.session_is_active = True

#     def read_data(self):
#         self.serial_devices.read_device(ModuleTypes.LORA_PIT)

#     def get_color(self, color):
#         colors = {
#             "HEADER": '\033[95m',
#             "OKBLUE": '\033[94m',
#             "OKCYAN": '\033[96m',
#             "OKGREEN": '\033[92m',
#             "WARNING": '\033[93m',
#             "FAIL": '\033[91m',
#             "ENDC": '\033[0m',
#             "BOLD": '\033[1m',
#             "UNDERLINE": '\033[4m'
#         }
#         return colors.get(color)

#     @staticmethod
#     def is_command(command_str):
#         commands = {attribute: value for attribute, value in Commands.__dict__.items() if not attribute.startswith('__')}
#         return command_str in commands.values()

#     def show_error(self, message):
#         QMessageBox.critical(self, "Error", message)

#     def show_info(self, message):
#         QMessageBox.information(self, "Information", message)


# if __name__ == "__main__":
#     app = QApplication(sys.argv)
#     window = CactusControlApp()
#     window.show()
#     sys.exit(app.exec())
# Buttons
from PyQt6 import QtCore, QtGui, QtWidgets
from CactusControlCLI import CactusControlCLI  # Import your CactusControlCLI class here

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 600)
        self.centralwidget = QtWidgets.QWidget(parent=MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        
        # Create an instance of CactusControlCLI
        self.cactus_cli = CactusControlCLI()

        # Buttons
        self.pushButton = QtWidgets.QPushButton(parent=self.centralwidget)
        self.pushButton.setGeometry(QtCore.QRect(20, 230, 75, 24))
        self.pushButton.setObjectName("pushButton")
        self.pushButton_2 = QtWidgets.QPushButton(parent=self.centralwidget)
        self.pushButton_2.setGeometry(QtCore.QRect(120, 230, 75, 24))
        self.pushButton_2.setObjectName("pushButton_2")
        self.pushButton_3 = QtWidgets.QPushButton(parent=self.centralwidget)
        self.pushButton_3.setGeometry(QtCore.QRect(220, 230, 75, 24))
        self.pushButton_3.setObjectName("pushButton_3")
        self.pushButton_4 = QtWidgets.QPushButton(parent=self.centralwidget)
        self.pushButton_4.setGeometry(QtCore.QRect(320, 230, 75, 24))
        self.pushButton_4.setObjectName("pushButton_4")
        self.pushButton_5 = QtWidgets.QPushButton(parent=self.centralwidget)
        self.pushButton_5.setGeometry(QtCore.QRect(410, 230, 75, 24))
        self.pushButton_5.setObjectName("pushButton_5")
        self.pushButton_6 = QtWidgets.QPushButton(parent=self.centralwidget)
        self.pushButton_6.setGeometry(QtCore.QRect(500, 230, 75, 24))
        self.pushButton_6.setObjectName("pushButton_6")
        self.pushButton_7 = QtWidgets.QPushButton(parent=self.centralwidget)
        self.pushButton_7.setGeometry(QtCore.QRect(590, 230, 75, 24))
        self.pushButton_7.setObjectName("pushButton_7")

        # Set button text
        self.retranslateUi(MainWindow)
        
        # Connect buttons to methods
        self.pushButton.clicked.connect(self.cactus_cli._print_commands)
        self.pushButton_2.clicked.connect(lambda: self.cactus_cli._begin_logging("BEGIN"))
        self.pushButton_3.clicked.connect(lambda: self.cactus_cli._end_logging("END"))
        self.pushButton_4.clicked.connect(lambda: self.cactus_cli._retrieve_logs("RETRIEVE"))
        self.pushButton_5.clicked.connect(lambda: self.cactus_cli._quit_program("QUIT"))
        self.pushButton_6.clicked.connect(self.cactus_cli._setup_session)
        self.pushButton_7.clicked.connect(self.cactus_cli._read_data)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(parent=MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 22))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(parent=MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.pushButton.setText(_translate("MainWindow", "HELP"))
        self.pushButton_2.setText(_translate("MainWindow", "BEGIN"))
        self.pushButton_3.setText(_translate("MainWindow", "END"))
        self.pushButton_4.setText(_translate("MainWindow", "RETRIEVE"))
        self.pushButton_5.setText(_translate("MainWindow", "QUIT"))
        self.pushButton_6.setText(_translate("MainWindow", "SETUP"))
        self.pushButton_7.setText(_translate("MainWindow", "READ"))

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec())