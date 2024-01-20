import serial

# Specify the COM port and baud rate
com_port = 'COM4'
baud_rate = 115200  # You may need to adjust this based on your Arduino sketch

# Open the serial connection
ser = serial.Serial(com_port, baud_rate, timeout=1)

try:
    while True:
        # Read data from the serial port
        data = ser.readline().decode('latin-1').strip()
        
        # Print the received data
        print(data)

except KeyboardInterrupt:
    # Close the serial connection when the program is interrupted
    ser.close()
    print("Serial connection closed.")
