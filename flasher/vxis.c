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
#include <fcntl.h>
#include <linux/usb/video.h>
#include <linux/uvcvideo.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "log.h"
#include "vxis.h"

enum {
    VXIS_XDATA_ADDR = 0x2,
    VXIS_XDATA_VALUE = 0x3,
    VXIS_ISP_ENABLE = 0x9,
    VXIS_FLASH_SECTOR_ERASE = 0xd,
    VXIS_FLASH_ADDRESS = 0xe,
    VXIS_FLASH_BUFFER = 0xf,
};

static int g_uvc_fd;
static ic_type_t g_curr_chip;
static int g_ignore_ioctl_error;

int vxis_open(const char* devpath)
{
    g_uvc_fd = open(devpath, O_RDWR | O_NONBLOCK, 0);
    if (g_uvc_fd < 0) {
        log_error("Can't open device\n");
        return 1;
    }
    return 0;
}

void vxis_close(void) {
    close(g_uvc_fd);
}

static int uvc_ctrl_get_len(uint8_t sel, uint16_t *len)
{
    int ret;
    __u16 size = 0;

    struct uvc_xu_control_query xquery = {
        .query = UVC_GET_LEN,
        .size = 2, /* size is always be 2 for the LEN query */
        .selector = sel,
        .unit = 0x4,
        .data = (__u8 *)&size
    };

    ret = ioctl(g_uvc_fd, UVCIOC_CTRL_QUERY, &xquery);
    if (ret < 0 && !g_ignore_ioctl_error) {
        log_error("UVCIOC_CTRL_QUERY ioctl failed: %d\n", ret);
        return 1;
    }

    *len = size;
    return 0;
}

static int uvc_ctrl_get_cur(uint8_t sel, size_t sz, uint8_t *buf)
{
    int ret;
    uint16_t ctrlen = 0;

    struct uvc_xu_control_query xquery = {
        .query = UVC_GET_CUR,
        .size = sz,
        .selector = sel,
        .unit = 0x4,
        .data = buf
    };

    ret = ioctl(g_uvc_fd, UVCIOC_CTRL_QUERY, &xquery);
    if (ret < 0 && !g_ignore_ioctl_error) {
        log_error("UVCIOC_CTRL_QUERY ioctl failed: %d\n", ret);
        return 1;
    }

    return 0;
}

static int uvc_ctrl_set_cur(uint8_t sel, size_t sz, uint8_t *buf)
{
    int ret;
    struct uvc_xu_control_query xquery = {
        .query = UVC_SET_CUR,
        .size = sz,
        .selector = sel,
        .unit = 0x4,
        .data = buf
    };

    ret = ioctl(g_uvc_fd, UVCIOC_CTRL_QUERY, &xquery);
    if (ret < 0 && !g_ignore_ioctl_error) {
        log_error("UVCIOC_CTRL_QUERY ioctl failed: %d\n", ret);
        return 1;
    }

    return 0;
}

int vxis_get_isp_status(uint8_t *val)
{
    int ret;

    ret = uvc_ctrl_get_cur(VXIS_ISP_ENABLE, 1, val);
    if (ret) {
        log_debug("get isp status failed\n");
        return 1;
    }

    return 0;
}

int vxis_enable_isp(void)
{
    int ret;
    uint8_t status;

    status = 0;
    ret = vxis_get_isp_status(&status);
    if (ret)
        return ret;

    if (status == 1) {
        log_debug("ISP already enabled\n");
        return 0;
    }

    status = 1;
    ret = uvc_ctrl_set_cur(VXIS_ISP_ENABLE, 1, &status);
    if (ret) {
        log_debug("set isp status failed\n");
        return 1;
    }

    ret = vxis_get_isp_status(&status);
    if (ret)
        return ret;
    if (status != 1) {
        log_debug("ISP enable failed. status=0x%02x\n", status);
        return 1;
    }

    return 0;
}

const char* vxis_ic2str(ic_type_t type)
{
    const char *str;

    switch(type) {
    case IC_SPCA_2610:
        str = "SPCA_2610";
        break;
    default:
        str = "Unknow IC";
    }

    return str;
}

int vxis_detect_ic (ic_type_t *type)
{
    int ret;
    uint8_t reg;

    ret = vxis_xdata_read(0x20ff, &reg);
    if (ret) {
        log_error("fail to read 0x20ff\n");
        return 1;
    }
    log_debug("reg 0x20ff = %02x\n", reg);

    if (reg & 0xf0) {
        *type = IC_SPCA_2610;
        return 0;
    }
    else {
        *type = IC_INVALID;
        return 0;
    }
}

int vxis_is_rom_mode(void)
{
    int ret;
    uint8_t reg;

    ret = vxis_xdata_read(0x2517, &reg);
    if (ret) {
        log_error("fail to read 0x2517\n");
        return 0;
    }
    log_debug("reg 0x2517 = %02x\n", reg);

    if (reg == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

int vxis_xdata_read(uint16_t addr, uint8_t *val)
{
    int ret;
    uint8_t u8addr[2] = {
        addr & 0xff,
        (addr >> 8) & 0xff
    };
    ret = uvc_ctrl_set_cur(VXIS_XDATA_ADDR, 2, u8addr);
    if (ret) {
        log_error("send register addr failed\n");
        return 1;
    }

    ret = uvc_ctrl_get_cur(VXIS_XDATA_VALUE, 1, val);
    if (ret) {
        log_error("get register value failed\n");
        return 1;
    }

    return 0;
}

int vxis_xdata_write(uint16_t addr, uint8_t val)
{
    int ret;
    uint8_t u8addr[2] = {
        addr & 0xff,
        (addr >> 8) & 0xff
    };
    uint8_t tempval = val;

    ret = uvc_ctrl_set_cur(VXIS_XDATA_ADDR, 2, &u8addr[0]);
    if (ret) {
        log_error("send register addr failed\n");
        return 1;
    }

    ret = uvc_ctrl_set_cur(VXIS_XDATA_VALUE, 1, &tempval);
    if (ret) {
        log_error("set register value failed\n");
        return 1;
    }

    return 0;
}

int vxis_xdata_write_ignore_error(uint16_t addr, uint8_t val)
{
    int ret;
    g_ignore_ioctl_error = 1;
    ret = vxis_xdata_write(addr, val);
    g_ignore_ioctl_error = 0;
    return ret;
}

int vxis_flash_get_id(uint8_t chipid[3], uint8_t spicmd)
{
    int ret;
    uint8_t cmd;

    cmd = 0xd;

    ret = uvc_ctrl_set_cur(7, 1, &cmd);
    if (ret) {
        log_error("send command failed\n");
        return 1;
    }

    cmd = spicmd;
    ret = uvc_ctrl_set_cur(8, 1, &cmd);
    if (ret) {
        log_error("send command failed\n");
        return 1;
    }

    ret = uvc_ctrl_get_cur(0x17, 3, chipid);
    if (ret) {
        log_error("get data failed\n");
        return 1;
    }

    return 0;
}

int vxis_flash_erase_sector(uint32_t addr)
{
    int ret;
    uint8_t u8addr[4] = {
        addr & 0xff,
        (addr >> 8) & 0xff,
        (addr >> 16) & 0xff,
        (addr >> 24) & 0xff,
    };

    ret = uvc_ctrl_set_cur(VXIS_FLASH_SECTOR_ERASE, 4, &u8addr[0]);
    if (ret) {
        log_error("fail to erase. addr=0x%08x\n", addr);
        return 1;
    }

    return 0;
}

int vxis_flash_get_buffer_size(size_t *len)
{
    int ret;
    uint16_t u16len = 0;

    ret = uvc_ctrl_get_len(VXIS_FLASH_BUFFER, &u16len);
    if (ret) {
        log_error("failed to get flash buffer size\n");
        return 1;
    }

    *len = u16len;
    return 0;
}

int vxis_flash_write(uint32_t addr, size_t sz, uint8_t *buf)
{
    int ret;
    uint8_t u8addr[4] = {
        addr & 0xff,
        (addr >> 8) & 0xff,
        (addr >> 16) & 0xff,
        (addr >> 24) & 0xff,
    };

    ret = uvc_ctrl_set_cur(VXIS_FLASH_ADDRESS, 4, &u8addr[0]);
    if (ret) {
        log_debug("fail to set address. addr=0x%08x\n", addr);
        return 1;
    }

    ret = uvc_ctrl_set_cur(VXIS_FLASH_BUFFER, sz, buf);
    if (ret) {
        log_debug("write buffer failed. addr=0x%08x\n", addr);
        return 1;
    }

    return 0;
}

int vxis_flash_read (uint32_t addr, size_t sz, uint8_t *buf)
{
    int ret;
    uint8_t u8addr[4] = {
        addr & 0xff,
        (addr >> 8) & 0xff,
        (addr >> 16) & 0xff,
        (addr >> 24) & 0xff,
    };

    ret = uvc_ctrl_set_cur(VXIS_FLASH_ADDRESS, 4, &u8addr[0]);
    if (ret) {
        log_debug("fail to set address. addr=0x%08x\n", addr);
        return 1;
    }

    ret = uvc_ctrl_get_cur(VXIS_FLASH_BUFFER, sz, buf);
    if (ret) {
        log_debug("read buffer failed. addr=0x%08x\n", addr);
        return 1;
    }

    return 0;
}
