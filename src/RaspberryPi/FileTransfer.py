from xmodem import XMODEM

def getc(size, timeout=1):
     return ser.read(size) or None

def putc(data, timeout=1):
     return ser.write(data)  # note that this ignores the timeout

s = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, bytesize=8, parity='N',     stopbits=1, timeout=None, xonxoff=0, rtscts=0)
s.open()

modem = XMODEM(getc, putc)

stream = open('file.txt', 'wb')
modem.recv(stream)
s.close()

