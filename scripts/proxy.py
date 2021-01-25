import struct, serial

class ProxyError(RuntimeError):
    pass

class SerialProxy:

    def __init__(self, dev, debug=False):
        self.opened = False
        self.open(dev)

    def open(self, dev):
        if self.opened:
            self.close()
        self.ser = serial.Serial(dev, 9600)
        self.opened=True

    def close(self):
        if self.opened:
            self.ser.close()
            self.opened = False

    def ping(self):
        self.ser.write(b'P')
        self.ser.flush()

        ret = self.ser.read(1)
        if ret != b'A':
            raise ProxyError("invalid respond")

    def readc(self, addr):
        cmd = struct.pack("<cH", b'c', addr)
        self.ser.write(cmd)
        return self.ser.read(1)[0]

    def readx(self, addr, length=1):
        cmd = struct.pack("<cH", b'x', addr)
        self.ser.write(cmd)
        return self.ser.read(1)[0]

    def readi(self, addr, length=1):
        cmd = struct.pack("<cB", b'i', addr)
        self.ser.write(cmd)
        return self.ser.read(1)[0]

    def writex(self, addr, val):
        cmd = struct.pack("<cHB", b'w', addr, val)
        self.ser.write(cmd)
        ret = self.ser.read(1)
        if ret != b'A':
            raise ProxyError("invalid respond")

    def writei(self, addr, val):
        cmd = struct.pack("<cBB", b'e', addr, val)
        self.ser.write(cmd)
        ret = self.ser.read(1)
        if ret != b'A':
            raise ProxyError("invalid respond")
