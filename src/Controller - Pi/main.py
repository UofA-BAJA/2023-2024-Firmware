from utilsImporter import adding_utils_to_path

adding_utils_to_path()

from SerialDevices import SerialDevices
from ConfigParser import Commands, ModuleTypes, DataTypes

from db.databaseInterfacer import get_BajaCloud_connection, insert_session

import time
import re
# ! lily was here
# ayo



class Controller:

    def __init__(self):
        self.serial_devices = SerialDevices()
        self.conn = get_BajaCloud_connection()
        self.session_id = None
        self.rows_lost = 0
        self.rows_added = 0

    def handleCommand(self, command_type_enum):

        if command_type_enum != Commands.RETRIEVE:
            self.serial_devices.sendCommandToAllDataModules(command_type_enum.name)
        else:
            for device in self.serial_devices._serial_devices:

                if (device != ModuleTypes.LORA_PIT) and (device != ModuleTypes.LORA_PI):

                    self.serial_devices._execute_single_command(Commands.RETRIEVE.name, device)
                    # serial_devices.read_file_data(device)
                    device_serial_obj = self.serial_devices._serial_devices[device]

                    self._get_datatypes_in_data(device_serial_obj)

                    self._insert_data_into_table(device_serial_obj)

                    self.serial_devices._execute_single_command("LISTENUP", ModuleTypes.LORA_PI)
                    for datatypes in self.datatypes:
                        self.serial_devices._execute_single_command(datatypes, ModuleTypes.LORA_PI)
                    self.serial_devices._execute_single_command("END", ModuleTypes.LORA_PI)

    def _get_datatypes_in_data(self, device_serial_obj):

        self.datatypes = []
        while True:
            device_output =  device_serial_obj.readline().decode('utf-8').strip()


            if "Micros" in device_output:
                for possible_datatypes in device_output.split(","):
                    if possible_datatypes in DataTypes.__members__:
                        print(f"Data type: {possible_datatypes}")
                        self.datatypes.append(possible_datatypes)

                    elif possible_datatypes == "Micros":
                        print(f"Time: {possible_datatypes}")
                    else:
                        print(f"Unknown data type: {possible_datatypes}")
                        continue

                break
            else:
                print(device_output)


    def _insert_data_into_table(self, device_serial_obj):
        while True:
            try:
                device_output =  device_serial_obj.readline().decode('utf-8').strip()
            except UnicodeDecodeError:
                print("UnicodeDecodeError, lost a row of data :(")
                self.rows_lost -= 1
                self.rows_added += 1
                continue
            # print(device_output)
            self.rows_added += 1
            if "Finished" in device_output:
                break

            # Split the device output by commas
            data_values = device_output.split(",")

            # Dynamically construct the columns and corresponding placeholders based on datatypes
            # Include 'ID' for the session ID and 'Time' column explicitly
            columns = ['ID', 'Time'] + self.datatypes
            placeholders = ', '.join(['?'] * len(columns))  # Generate placeholders for SQL parameters

            # Construct the SQL insert statement
            sql = f"INSERT INTO BajaCloudData ({', '.join(columns)}) VALUES ({placeholders})"

            # Execute the insert statement with the corresponding values
            try:
                cur = self.conn.cursor()

                # Construct data_to_insert to include the session_id, the 'Time' value, and the rest of the data
                data_to_insert = [self.session_id] + [data_values[0]]  # Start with self.session_id and 'Time' value

                # Now add the rest of the values based on the datatype order
                # Skip first datatype as it corresponds to 'Time', already added
                for dtype in self.datatypes:
                    index = self.datatypes.index(dtype) + 1  # Adjust index to match values after 'Time'
                    if index < len(data_values):  # Ensure index is within bounds
                        data_to_insert.append(data_values[index])
                    else:
                        # If the data type is missing in the current line, append NULL (or an appropriate default)
                        data_to_insert.append(None)

                # Ensure `data_to_insert` matches the order and number of `columns`
                # print(f"SQL: {sql}")
                cur.execute(sql, data_to_insert)
                self.conn.commit()
            except Exception as e:
                print(f"Failed to insert data: {e}")

            print(f"Rows added to database: {self.rows_added}, lost rows: {self.rows_lost}")

    def run(self):
        if (ModuleTypes.LORA_PIT in self.serial_devices._serial_devices):
            self.serial_devices._serial_devices[ModuleTypes.LORA_PIT].close()

        lora_device = self.serial_devices.get_device(ModuleTypes.LORA_PI)

        if (lora_device) == None:
            print("No LORA PI DEVICE FOUND!!!!!!")
            while(1): pass

        while True:
            lora_serial_input = self.serial_devices._wait_for_lora_serial_input()

            try:
                parsed_input = Controller.parse_input(lora_serial_input)
                command_type_enum = Commands[parsed_input]
                self.handleCommand(command_type_enum=command_type_enum)
                print(f"Finished handling command: {command_type_enum} successfully.")
            except KeyError:
                print(f"Serial input: {lora_serial_input} is not a valid command.")

            if "SESSION" in lora_serial_input:
                actual_text = Controller.parse_input(lora_serial_input).replace("SESSION:", "")

                self.session_id = insert_session(self.conn, actual_text)

                print(f"ADDING SESSION: '{actual_text}' INTO DATABASE")
                print(f"CURRENT SESSION ID IS: {self.session_id}")
        
        self.conn.close()


    def parse_input(raw):
        # print(f"idk {command}")
        pattern = re.compile(r'{(.*?)}')
        # Find all matches in the text
        matches = pattern.findall(raw)

        return matches[0]

        


if __name__ == "__main__":
    c = Controller()

    c.run()