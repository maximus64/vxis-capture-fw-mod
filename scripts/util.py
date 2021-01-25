import sys

def hexdump(buf, base=0):
    for i in range(0, len(buf), 16):
        sys.stdout.write("%08x | " % (base+i))
        for j in range(0, 16):
            if i+j > len(buf):
                sys.stdout.write("   ")
            else:
                sys.stdout.write("%02x " % buf[i+j])
        sys.stdout.write("| ")
        for j in range(0, 16):
            if i+j > len(buf):
                sys.stdout.write(" ")
            else:
                if(buf[i+j] >= 33 and buf[i+j] <= 126):
                    sys.stdout.write("%c" % buf[i+j])
                else:
                    sys.stdout.write(".")
        sys.stdout.write("\n")

def hexdiff(buf, buf2, base=0):
    diff = []
    for i in range(0, len(buf), 16):
        sys.stdout.write("%08x | " % (base+i))
        for j in range(0, 16):
            if i+j > len(buf):
                sys.stdout.write("   ")
            else:
                if buf[i+j] != buf2[i+j]:
                    sys.stdout.write("\x1b[31m")
                    sys.stdout.write("%02x " % buf[i+j])
                    sys.stdout.write("\x1b[0m")
                    diff.append(i+j)
                else:
                    sys.stdout.write("%02x " % buf[i+j])
        sys.stdout.write("| ")
        for j in range(0, 16):
            if i+j > len(buf):
                sys.stdout.write(" ")
            else:
                if(buf[i+j] >= 33 and buf[i+j] <= 126):
                    sys.stdout.write("%c" % buf[i+j])
                else:
                    sys.stdout.write(".")
        sys.stdout.write("\n")
    for x in diff:
        stra = ''
        strb = ''
        for i in range(8):
            a = '1' if (buf[x] & (1 << (7-i))) else '0'
            b = '1' if (buf2[x] & (1 << (7-i))) else '0'
            if a != b:
                stra += "\x1b[31m"
                stra += a
                stra += "\x1b[0m"
                strb += "\x1b[31m"
                strb += b
                strb += "\x1b[0m"
            else:
                stra += a
                strb += b
        print("Address 0x%04x different: 0x%02x (0b%s) != 0x%02x (0b%s)" %
            (
                base + x,
                buf[x], stra,
                buf2[x], strb,
            )
        )
