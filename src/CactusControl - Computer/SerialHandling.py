import serial
import serial.tools.list_ports
import time

from util.ConfigParser import Commands, ModuleTypes 

lora_device = None

def get_lora_pit_serial_device():
    global lora_device
    ports = list(serial.tools.list_ports.comports())
    port_paths = []
    for p in ports:
        # Example of filtering out Bluetooth devices by checking for a keyword in the description
        # Adjust the keyword according to your needs
        if 'Bluetooth' not in p.description:
            print(f"Device detected: {p.device}")
            port_paths.append(p.device)
        else:
            print(f"Bluetooth device ignored: {p.device}")

    for path in port_paths:
        ser = serial.Serial(path, 115200, timeout=5)

        ser.setDTR(False) # Drop DTR
        time.sleep(0.023)    # Read somewhere that 22ms is what the UI does.
        ser.setDTR(True)  # UP the DTR back

        ser.flushInput()
        ser.flushOutput()
        time.sleep(.3)

        is_device_ready = False
        print(f"\nDevice Output on {ser.port}:")
        while not is_device_ready:
            device_output =  ser.readline().decode('utf-8').strip()                
            print(device_output)
            if device_output == "Ready":
                is_device_ready = True
        print(f"End of Device Output on {ser.port}\n")

                
        ser.write(f"<{Commands.SENDTYPE.name}>".encode('utf-8'))
        
        dev_type = None
        while ser.in_waiting == 0:
            dev_type = ser.readline().decode('utf-8').strip()
            break

        if dev_type == ModuleTypes.LORA_PIT.name:
            print(f"Found LORA_PIT device on {ser.port}")
            return ser

        else:
            print(f"Device type is not PIT LORA: {dev_type}")
            ser.close()   