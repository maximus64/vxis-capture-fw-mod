#!/usr/bin/env python3
import atexit, sys, os, struct, code, traceback, readline, rlcompleter, time
import __main__
from util import hexdump
from pyftdi.ftdi import Ftdi
from pyftdi.i2c import I2cController,I2cNackError

class HistoryConsole(code.InteractiveConsole):
    def __init__(self, locals=None, filename="<console>",
                 histfile=os.path.expanduser("~/.ftdi-i2c-history")):
        code.InteractiveConsole.__init__(self, locals, filename)
        self.init_history(histfile)

    def init_history(self, histfile):
        readline.parse_and_bind("tab: complete")
        if hasattr(readline, "read_history_file"):
            try:
                readline.read_history_file(histfile)
            except FileNotFoundError:
                pass
            atexit.register(self.save_history, histfile)

    def save_history(self, histfile):
        readline.set_history_length(1000)
        readline.write_history_file(histfile)

    def showtraceback(self):
        type, value, tb = sys.exc_info()
        traceback.print_exception(type, value, tb)

if len(sys.argv) < 2:
    print("Usage: %s ftdi_url" % sys.argv[0])
    Ftdi.show_devices()
    exit(1)

VS9989_I2C_ADDR = 0x44

i2c = I2cController()

i2c.configure(url=sys.argv[1], frequency=100000)

port = i2c.get_port(VS9989_I2C_ADDR)

def scan_bus():
    print("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f")
    for i in range(0, 128, 16):
        print("%02x: " % i, end = '')
        for j in range(0, 16):
            found = True
            try:
                i2c.get_port(i+j).read(readlen=0, relax=True, start=True)
            except I2cNackError:
                found = False

            if found:
                print("%02x " % (i+j), end = '')
            else:
                print("-- ", end = '')
        print("")

def diff(ina, inb):
    l = min(len(ina), len(inb))
    for i in range(l):
        if ina[i] != inb[i]:
            print("Differ [0x%x] %02x != %02x" % (i, ina[i], inb[i]))

def test_rw(reg):
    a = port.read_from(reg, 1)[0]
    b = (~a & 0xff)
    port.write_to(reg, [b])
    c = port.read_from(reg, 1)[0]
    if c == b:
        ret = True
    else:
        ret = False
    port.write_to(reg, [a])
    return ret

def test_inc(reg, delay=0.1):
    for i in range(256):
        print(i)
        port.write_to(reg, [i])
        time.sleep(delay)

def test_bitflip(reg, delay=1):
    a = port.read_from(reg, 1)[0]
    print("current: 0x%02x" % a)
    for i in range(8):
        val = a ^ (1<<i)
        print("Bit %d, Value: 0x%02x" % (i, val))
        port.write_to(reg, [val])
        if delay <= 0:
            input("Enter to continue...")
        else:
            time.sleep(delay)
        print("restore original: 0x%02x" % a)
        port.write_to(reg, [a])
        if delay <= 0:
            input("Enter to continue...")
        else:
            time.sleep(delay)

def terminate():
    i2c.terminate()
    exit()

locals = __main__.__dict__

HistoryConsole(locals).interact("Have fun!")
