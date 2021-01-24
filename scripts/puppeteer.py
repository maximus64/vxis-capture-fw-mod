#!/usr/bin/env python3
import atexit, sys, os, struct, code, traceback, readline, rlcompleter, time
import __main__
import serial
from util import hexdump, hexdiff
from proxy import SerialProxy

class HistoryConsole(code.InteractiveConsole):
    def __init__(self, locals=None, filename="<console>",
                 histfile=os.path.expanduser("~/.usb3-cap-puppet-history")):
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
    print("Usage: %s TTY_DEVICE" % sys.argv[0])
    exit(1)

s = SerialProxy(sys.argv[1])

print("Pinging device...")
s.ping()

blacklist_reg = [
    0x204c, #control blue led
    0x2057, #bit[0] control UART RX output
    0x2056, #bit[0] control UART RX pinmode

# folowing cause hang
    0x2010,
]

def read_xdata(addr, length):
    if (addr + length) > 0x10000:
        raise ValueError("addr/len too large")
    array = bytearray(length)
    for i in range(length):
        print("Reading addres 0x%04x" % (addr+i))
        val = s.readx(addr+i)
        array[i] = val
    return array

def read_code(addr, length):
    if (addr + length) > 0x10000:
        raise ValueError("addr/len too large")
    array = bytearray(length)
    for i in range(length):
        print("Reading addres 0x%04x" % (addr+i))
        val = s.readc(addr+i)
        array[i] = val
    return array

def read_iram(addr, length):
    if (addr + length) > 0x100:
        raise ValueError("addr/len too large")
    array = bytearray(length)
    for i in range(length):
        print("Reading addres 0x%04x" % (addr+i))
        val = s.readi(addr+i)
        array[i] = val
    return array


def compare_xdata(addr, length):
    data = read_xdata(addr, length)
    with open("../../firmware/xdata_dump_parsed.bin", "rb") as a:
        a.seek(addr)
        golden = a.read(length)
    hexdiff(data, golden, addr)

def apply_xdata(addr, length):
    data = read_xdata(addr, length)
    with open("../../firmware/xdata_dump_parsed.bin", "rb") as a:
        a.seek(addr)
        golden = a.read(length)
    for i in range(length):
        if data[i] != golden[i]:
            print("Address 0x%04x different: 0x%02x != 0x%02x" % (addr+i, data[i], golden[i]))
            if (addr+i) in blacklist_reg:
                print("Blacklisted - SKIP")
                continue
            yn = input("Apply? [y]/n: ").lower().strip()
            if yn == 'y' or yn == '':
                s.writex(addr+i, golden[i])
                print("Applied")
            else:
                print("Skip")


locals = __main__.__dict__

HistoryConsole(locals).interact("Have fun!")
