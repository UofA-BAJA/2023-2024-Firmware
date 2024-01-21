import serial
import serial.tools.list_ports
import time

# https://www.youtube.com/watch?v=jU_b8WBTUew <------ watch this video

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
    time.sleep(3)
    for device in serial_devices:
        device.reset_input_buffer()
    return serial_devices

def start_logging():
    # send command to nano's to start logging onto their SD cards
    pass

def end_logging():
    # send command to nano's to stop logging onto their SD cards
    pass

def retrieve_logs():
    # retrieve data from nano's SD cards
    pass

def receive_data(serial_devices):
    try:
        while True:
            time.sleep(0.05)
            for ser in serial_devices:
                # ser.in_waiting is the num of bytes in the buffer. 
                if ser.in_waiting > 0:
                    data = ser.readline().decode('utf-8').rstrip()
                    print(data)
    except KeyboardInterrupt:
        for ser in serial_devices:
            ser.close()
        print("\nSerial Devices Closed")

def main():
    port_paths = get_port_paths()
    ser_devices = get_serial_devices(port_paths)
    receive_data(ser_devices)

if __name__ == "__main__":
    main()