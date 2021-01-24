#!/usr/bin/env python3
import sys, os, struct
from py8051core import ffi, lib
from py8051 import Emulator8051
from proxy import SerialProxy

if len(sys.argv) < 2:
    print("Usage: %s /dev/ttyX" % sys.argv[0])
    exit(1)

fin = open('../../firmware/full_dump.bin', 'rb')
s = SerialProxy(sys.argv[1])

print("Pinging device...")
s.ping()

e = Emulator8051()
print("Load firmware to memory")
buf = fin.read()
if len(buf) > e.emu.mCodeMemSize:
    raise RuntimeError("fw too large")
ffi.memmove(e.code_memory, buf, len(buf))
del buf


@ffi.def_extern()
def em8051xwrite_callback(aCPU, aAddress, aValue):
    if aAddress < 0x0c00: #External RAM. might need to forward some for DMA
        aCPU.mExtData[aAddress] = aValue
        return
    print("[XDATA] PC:%04x write address: 0x%04x value: %02x" % (aCPU.mPC, aAddress, aValue))
    #aCPU.mExtData[aAddress] = aValue
    s.writex(aAddress, aValue)

@ffi.def_extern()
def em8051xread_callback(aCPU, aAddress):
    if aAddress < 0x0c00: #External RAM. might need to forward some for DMA
        return aCPU.mExtData[aAddress]
    got = s.readx(aAddress)
    print("[XDATA] PC:%04x read address: 0x%04x got: %02x" % (aCPU.mPC, aAddress, got))
    return got

e.emu.xread = lib.em8051xread_callback
e.emu.xwrite = lib.em8051xwrite_callback

e.set_r(7, 0x88)
e.set_r(5, 0x11)
e.set_r(3, 0xaa)

#e.set_PC(0xcc35) #i2c_write
e.set_PC(0xf210) #i2c_read
e.set_SP(0x10)

#setup register to enable i2c
# s.writex(0x2004,0x34) # enable i2c ?
# s.writex(0x2790,0xff) # needed to make i2c read work. otherwise get all 0


while True:
    if e.PC() == 0xcc47: #i2c_write return address
        break
    if e.PC() == 0xf222: #i2c_read return address
        break
    #print("[OPCODE] %04x: %s" % (e.PC(), e.decode(e.PC())))
    #print("[REG] r0-7:%02x %02x %02x %02x %02x %02x %02x %02x A:%02x DPTR: %04x" % (
    #       e.r(0), e.r(1), e.r(2), e.r(3), e.r(4), e.r(5), e.r(6), e.r(7), e.ACC(), e.DPTR()))
    e.step()

print("[REG] r0-7:%02x %02x %02x %02x %02x %02x %02x %02x A:%02x DPTR: %04x" % (
        e.r(0), e.r(1), e.r(2), e.r(3), e.r(4), e.r(5), e.r(6), e.r(7), e.ACC(), e.DPTR()))
 