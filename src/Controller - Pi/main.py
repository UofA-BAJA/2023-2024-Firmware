from UofA_BAJA_2023_2024_common.enums import Commands, ModuleTypes, WirelessNodeTypes, DataTypes, MessageHeaders
from UofA_BAJA_2023_2024_common.SerialDevices import SerialDevices
from UofA_BAJA_2023_2024_common.Messages import construct_message
from db.databaseInterfacer import get_BajaCloud_connection, insert_session

import time
import re
import json
# ! lily was here
# ayo



class Controller:

    def __init__(self):
        self.serial_devices = SerialDevices()
        self.conn = get_BajaCloud_connection()
        self.session_id = None
        self.rows_lost = 0
        self.rows_added = 0

        self.response = ""

    def handleCommand(self, command_type_enum):

        if command_type_enum != Commands.RETRIEVE:
            self.serial_devices.sendCommandToAllDataModules(command_type_enum)
        else:

            for device in self.serial_devices._serial_devices:
                print(device)
                if (device != ModuleTypes.LORA_PIT) and (device != ModuleTypes.LORA_PI):

                    
                    self.serial_devices._execute_single_command(Commands.RETRIEVE, device)
                    # serial_devices.read_file_data(device)
                    device_serial_obj = self.serial_devices._serial_devices[device]

                    self._get_datatypes_in_data(device_serial_obj)

                    self._insert_data_into_table(device_serial_obj)

                    datatypes_as_json = json.dumps({"datatypes": self.datatypes})

                    self.send_response_to_pit(datatypes_as_json)
                    
    def send_response_to_pit(self, response_message_str: str):

        response_message =  construct_message(target_device=WirelessNodeTypes.COMPUTER, message=response_message_str )
        self.serial_devices._execute_single_command(response_message, ModuleTypes.LORA_PI)

       
    def _get_datatypes_in_data(self, device_serial_obj):
        valid_data_types = {attr for attr in dir(DataTypes) if not attr.startswith('__')}

        self.datatypes = []

        empty_output_counter = 0
        while True:
            device_output =  device_serial_obj.readline().decode('utf-8').strip()


            if "Micros" in device_output:
                for possible_datatypes in device_output.split(","):
                    if possible_datatypes in valid_data_types:
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

            if device_output == "":
                empty_output_counter += 1

            if empty_output_counter > 10:
                print("No data types found in the data stream")
                break
            

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

    def get_datatype_and_send_to_pit(self, data_query):
        print(f"processig data query: {data_query}")
        data_query_json = json.loads(data_query)

        selected_datatype = data_query_json["data-query"]["selected-datatype"]

        sql_query = f"SELECT {selected_datatype} FROM BajaCloudData WHERE ID = ?;"

        # other_sql_query = f"SELECT * FROM BajaCloudData WHERE ID = {self.session_id};"
        # print(other_sql_query)
        c = self.conn.cursor()
        c.execute(sql_query, (self.session_id,))

        results = c.fetchall()

        for row in results:
            print(row)

        # Combine the values into a single string, separated by commas, and ensure it doesn't exceed 512 characters
        combined_string = ", ".join(str(row[0]) for row in results)

        # Ensure the combined string does not exceed 512 characters
        if len(combined_string) > 256:
            combined_string = combined_string[:256-3] + "..."

        self.send_response_to_pit(json.dumps({"data-packet": combined_string}))            

    def run(self):
        if (ModuleTypes.LORA_PIT in self.serial_devices._serial_devices):
            self.serial_devices._serial_devices[ModuleTypes.LORA_PIT].close()

        lora_device = self.serial_devices.get_device(ModuleTypes.LORA_PI)

        if (lora_device) == None:
            print("No LORA PI DEVICE FOUND!!!!!!")
            while(1): pass

        while True:
            lora_serial_input = self.serial_devices._wait_for_lora_serial_input()

            
            parsed_message = Controller.parse_input(lora_serial_input)
            
            if (Controller.check_if_input_warrants_a_response(parsed_message)):
                self.send_response_to_pit(MessageHeaders.PYTHON_MESSAGE)

            if (Controller.is_command(parsed_message)):
                self.handleCommand(command_type_enum=parsed_message)
                self.send_response_to_pit(json.dumps({"message" : f"success for command {parsed_message}"}))
            elif (Controller.is_data_query(parsed_message)):
                self.get_datatype_and_send_to_pit(parsed_message)
            else:
                print(f"Serial input: {parsed_message} is not a valid command.")

            if "SESSION" in lora_serial_input:
                actual_text = Controller.parse_input(lora_serial_input).replace("SESSION:", "")

                self.session_id = insert_session(self.conn, actual_text)

                print(f"ADDING SESSION: '{actual_text}' INTO DATABASE")
                self.send_response_to_pit(json.dumps({"message" :f"ADDED SESSION: '{actual_text}' INTO DATABASE"}))

                print(f"CURRENT SESSION ID IS: {self.session_id}")

            if (Controller.check_if_input_warrants_a_response(parsed_message)):
                self.send_response_to_pit(MessageHeaders.PYTHON_MESSAGE)
        self.conn.close()


    def parse_input(raw):
        matches = []
        bracketed_message = re.findall(r'<(.*?)>', raw)

        # Now extract the 'mesg' part
        for message in bracketed_message:
            mesg_part = re.search(r'-mesg:(.*?)!', message)
            if mesg_part:
                matches.append(mesg_part.group(1))  

        return matches[0]

    
    def is_command(command_str):
        commands = {attribute: value for attribute, value in Commands.__dict__.items() if not attribute.startswith('__')}
        return command_str in commands.values()
    
    def is_data_query(input_str):
        try:
            json_query = json.loads(input_str)
            
            if "data-query" in json_query:
                return True
        except json.JSONDecodeError:
            return False

    def check_if_input_warrants_a_response(input_str):
        if "SESSION" in input_str or Controller.is_command(input_str) or Controller.is_data_query(input_str):
            return True
        else:
            return False
        
if __name__ == "__main__":
    c = Controller()

    c.run()