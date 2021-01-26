/*
 * Copyright © 2021 Khoa Hoang <admin@khoahoang.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "vxis.h"

static int print_usage(const char *s)
{
    fprintf(stderr, "Usage: %s (-r addr) (-w addr value) /dev/videoX\n\n", s);
    fprintf(stderr, "Read register 0x2170:\n");
    fprintf(stderr, "    %s -r 0x2170 /dev/video0\n", s);
    fprintf(stderr, "Write 0x80 to register 0x2170:\n");
    fprintf(stderr, "    %s -w 0x2170 0x20 /dev/video0\n", s);
    return EXIT_FAILURE;
}

int main (int argc, char *argv[])
{
    const char *devpath;
    int mode; /* 0 - read, 1 - write */
    int ret;
    uint16_t addr;
    uint8_t value;

    if (argc < 4)
        return print_usage(argv[0]);

    if (strcmp(argv[1], "-r") == 0) {
        mode = 0;
        addr = strtoul(argv[2], NULL, 0);
        value = 0;
        devpath = argv[3];
    }
    else if (strcmp(argv[1], "-w") == 0) {
        mode = 1;
        if (argc < 5)
            return print_usage(argv[0]);
        addr = strtoul(argv[2], NULL, 0);
        value = strtoul(argv[3], NULL, 0);
        devpath = argv[4];
    }
    else {
        return print_usage(argv[0]);
    }

    ret = vxis_open(devpath);
    if (ret)
        return EXIT_FAILURE;

    switch (mode) {
    case 0: /* read */
        ret = vxis_xdata_read(addr, &value);
        if (ret) {
            vxis_close();
            return EXIT_FAILURE;
        }
        printf("%02x\n", value);
        break;
    case 1: /* write */
        ret = vxis_xdata_write(addr, value);
        if (ret) {
            vxis_close();
            return EXIT_FAILURE;
        }
        break;
    default:
        break;
    }

    vxis_close();
    return EXIT_SUCCESS;
}
