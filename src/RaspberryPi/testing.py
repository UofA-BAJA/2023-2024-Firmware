import serial

def get_port_paths():
    ports = list(serial.tools.list_ports.comports())
    port_paths = []
    for p in ports:
        print(p.device)
        port_paths.append(p.device)
    return port_paths

def get_serial_devices(port_paths):
    serial_devices = []
    for path in port_paths:
        ser = serial.Serial(path, 9600)
        serial_devices.append(ser)
    return serial_devices

def start_logging():
    # send command to nano's to start logging onto their SD cards
    pass

def end_logging():
    # send command to nano's to stop logging onto their SD cards
    pass

def retrieve_data():
    # retrieve data from nano's SD cards
    pass


def main():
    port_paths = get_port_paths()
    ser_devices = get_serial_devices(port_paths)

    while True:
        for ser in ser_devices:
            data = ser.readline()
            print(data)

if __name__ == "__main__":
    main()