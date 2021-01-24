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

#define VS9989_I2C_SLAVE 0x88

static int print_usage(const char *s)
{
    fprintf(stderr, "Usage: %s (-r addr) (-w addr value) /dev/videoX\n\n", s);
    fprintf(stderr, "Read register 0x11:\n");
    fprintf(stderr, "    %s -r 0x11 /dev/video0\n", s);
    fprintf(stderr, "Write 0x20 to register 0x11:\n");
    fprintf(stderr, "    %s -w 0x11 0x20 /dev/video0\n", s);
    return EXIT_FAILURE;
}

static int vs9989_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t status = 0;
    int cnt;

    //NOTE: will break if FW try to do i2c between these command
    if (vxis_xdata_write(0x2600, VS9989_I2C_SLAVE)) goto err;
    if (vxis_xdata_write(0x2601, 0x04)) goto err;
    if (vxis_xdata_write(0x2603, 0x00)) goto err;
    if (vxis_xdata_write(0x2608, 0x02)) goto err;
    if (vxis_xdata_write(0x2610, reg)) goto err;
    if (vxis_xdata_write(0x2611, val)) goto err;

    cnt = 10;
    while (cnt--) {
        if (vxis_xdata_read(0x2650, &status)) goto err;
        if (status == 0)
            return 0;
    }
err:
    return 1;
}

static int vs9989_read_reg(uint8_t reg, uint8_t *val)
{
    int cnt;
    uint8_t status = 0;
    
    //NOTE: will break if FW try to do i2c between these command
    /*
     * This is to enable the i2c port
     */
    if (vxis_xdata_write(0x2004, 0x34)) goto err;
    if (vxis_xdata_read(0x2790, &status)) goto err;
    status &= ~(1<<6); /* clear bit 6 */
    if (vxis_xdata_write(0x2790, 0xb1)) goto err;

    if (vxis_xdata_write(0x2600, VS9989_I2C_SLAVE)) goto err;
    if (vxis_xdata_write(0x2601, 0x06)) goto err;
    if (vxis_xdata_write(0x2603, 0x00)) goto err;
    if (vxis_xdata_write(0x2608, 0x01)) goto err; //write len
    if (vxis_xdata_write(0x2609, 0x01)) goto err; //read len
    if (vxis_xdata_write(0x2610, reg)) goto err; //write data
    if (vxis_xdata_write(0x2602, 0x13)) goto err;

    cnt = 10;
    while (cnt--) {
        if (vxis_xdata_read(0x2650, &status)) goto err;
        if (status == 0)
            break;
    }
    if (!cnt)
        goto err;

    /* read value form i2c */
    if (vxis_xdata_read(0x2610, &val[0])) goto err;

    cnt = 10;
    while (cnt--) {
        if (vxis_xdata_read(0x2650, &status)) goto err;
        if (status == 0)
            return 0;
    }
err:
    return 1;
}

int main (int argc, char *argv[])
{
    const char *devpath;
    int mode; /* 0 - read, 1 - write */
    int ret;
    uint8_t addr, value;

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
        ret = vs9989_read_reg(addr, &value);
        if (ret)
            return EXIT_FAILURE;
        printf("Value=%02x\n", value);
        break;
    case 1: /* write */
        ret = vs9989_write_reg(addr, value);
        if (ret)
            return EXIT_FAILURE;
        break;
    default:
        break;
    }

    return EXIT_SUCCESS;
}
