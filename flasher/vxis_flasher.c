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
#include <getopt.h>
#include <fcntl.h>
#include <linux/usb/video.h>
#include <linux/uvcvideo.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEBUG

#ifdef DEBUG
#define log_debug(fmt, args...) printf("[DEBUG] %s: %d: " fmt, __func__, __LINE__, ## args)
#else
#define log_debug(fmt, args...)
#endif
#define log_error(fmt, args...) fprintf(stderr, "[ERROR] %s: %d: " fmt, __func__, __LINE__, ## args)
#define log_warn(fmt, args...) printf("[WARN] %s: %d: " fmt, __func__, __LINE__, ## args)
#define log_info(fmt, args...) printf(fmt, ## args)
#define ARRAY_SIZE(a) (sizeof(a) /sizeof(*(a)))

typedef enum {
    IC_INVALID,
    IC_SPCA_2610,
    IC_MAX
} ic_type_t;

typedef struct {
    const char *vendor;
    const char *part;
    uint32_t manufacture_id;
    uint32_t device_id;
    uint32_t sector_size;
    uint32_t sector_count;
} flash_chip_t;

static const flash_chip_t flash_info[] = {
    {
        .vendor         = "PMC",
        .part           = "Pm25LQ512B",
        .manufacture_id = 0x7f9d,
        .device_id      = 0x20,
        .sector_size    = 4096,
        .sector_count   = 16
    },
    {0}
};
static const int flash_info_count = ARRAY_SIZE(flash_info);

static int g_uvc_fd;
static ic_type_t g_curr_chip;
static int g_ignore_ioctl_error;

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

static int flash_get_id(uint8_t chipid[3], uint8_t spicmd)
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

static int read_reg(uint16_t addr, uint8_t *val)
{
    int ret;
    uint8_t u8addr[2] = {
        addr & 0xff,
        (addr >> 8) & 0xff
    };
    ret = uvc_ctrl_set_cur(2, 2, u8addr);
    if (ret) {
        log_error("send register addr failed\n");
        return 1;
    }

    ret = uvc_ctrl_get_cur(3, 1, val);
    if (ret) {
        log_error("get register value failed\n");
        return 1;
    }

    return 0;
}

static int write_reg(uint16_t addr, uint8_t val)
{
    int ret;
    uint8_t u8addr[2] = {
        addr & 0xff,
        (addr >> 8) & 0xff
    };
    uint8_t tempval = val;

    ret = uvc_ctrl_set_cur(2, 2, &u8addr[0]);
    if (ret) {
        log_error("send register addr failed\n");
        return 1;
    }

    ret = uvc_ctrl_set_cur(3, 1, &tempval);
    if (ret) {
        log_error("set register value failed\n");
        return 1;
    }

    return 0;
}

static int get_isp_status(uint8_t *val) {
    int ret;

    ret = uvc_ctrl_get_cur(9, 1, val);
    if (ret) {
        log_debug("get isp status failed\n");
        return 1;
    }

    return 0;
}

static int enable_isp(void) {
    int ret;
    uint8_t status;

    status = 0;
    ret = get_isp_status(&status);
    if (ret)
        return ret;

    if (status == 1) {
        log_debug("ISP already enabled\n");
        return 0;
    }

    status = 1;
    ret = uvc_ctrl_set_cur(9, 1, &status);
    if (ret) {
        log_debug("set isp status failed\n");
        return 1;
    }

    ret = get_isp_status(&status);
    if (ret)
        return ret;
    if (status != 1) {
        log_debug("ISP enable failed. status=0x%02x\n", status);
        return 1;
    }

    return 0;
}

static const char* ic_type_to_str(ic_type_t type){
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

static int detect_ic_type (ic_type_t *type) {
    int ret;
    uint8_t reg;

    ret = read_reg(0x20ff, &reg);
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

static int is_in_rom_mode(void) {
    int ret;
    uint8_t reg;

    ret = read_reg(0x2517, &reg);
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

static int flash_erase_sector(uint32_t addr) {
    int ret;
    uint8_t u8addr[4] = {
        addr & 0xff,
        (addr >> 8) & 0xff,
        (addr >> 16) & 0xff,
        (addr >> 24) & 0xff,
    };

    ret = uvc_ctrl_set_cur(0xd, 4, &u8addr[0]);
    if (ret) {
        log_error("fail to erase. addr=0x%08x\n", addr);
        return 1;
    }

    return 0;
}

static int flash_get_buffer_size(size_t *len) {
    int ret;
    uint16_t u16len = 0;

    ret = uvc_ctrl_get_len(0xf, &u16len);
    if (ret) {
        log_error("failed to get flash buffer size\n");
        return 1;
    }

    *len = u16len;
    return 0;
}

static int flash_write(uint32_t addr, size_t sz, uint8_t *buf)
{
    int ret;
    uint8_t u8addr[4] = {
        addr & 0xff,
        (addr >> 8) & 0xff,
        (addr >> 16) & 0xff,
        (addr >> 24) & 0xff,
    };

    ret = uvc_ctrl_set_cur(0xe, 4, &u8addr[0]);
    if (ret) {
        log_debug("fail to set address. addr=0x%08x\n", addr);
        return 1;
    }

    ret = uvc_ctrl_set_cur(0xf, sz, buf);
    if (ret) {
        log_debug("write buffer failed. addr=0x%08x\n", addr);
        return 1;
    }

    return 0;
}

static int flash_read (uint32_t addr, size_t sz, uint8_t *buf)
{
    int ret;
    uint8_t u8addr[4] = {
        addr & 0xff,
        (addr >> 8) & 0xff,
        (addr >> 16) & 0xff,
        (addr >> 24) & 0xff,
    };

    ret = uvc_ctrl_set_cur(0xe, 4, &u8addr[0]);
    if (ret) {
        log_debug("fail to set address. addr=0x%08x\n", addr);
        return 1;
    }

    ret = uvc_ctrl_get_cur(0xf, sz, buf);
    if (ret) {
        log_debug("read buffer failed. addr=0x%08x\n", addr);
        return 1;
    }

    return 0;
}

static const flash_chip_t* flash_get_info(uint8_t manuid1, uint8_t manuid2, uint8_t deviceid)
{
    int i;
    uint32_t manu, id;

    manu = manuid1 | manuid2 << 8;
    id = deviceid;

    for (i = 0; i < flash_info_count; i++) {
        if (flash_info[i].device_id == id &&
            flash_info[i].manufacture_id == manu) {
            return &flash_info[i];
        }
    }

    return NULL;
}

static int reset_to_rom(void) {
    int ret;
    uint8_t reg;

    switch (g_curr_chip) {
    case IC_SPCA_2610:
        ret = read_reg(0x2517, &reg);
        if (ret) {
            log_error("fail to read 0x2517\n");
            return 1;
        }
        log_debug("reg 0x2517 = %02x\n", reg);

        /* register 0x2517 is 0 in rom boot and 4 in spi boot? */
        if (reg == 0) {
            log_debug("already in ROM mode\n");
        }
        else if (reg == 4) {
            ret = read_reg(0x20eb, &reg);
            ret |= write_reg(0x20eb, reg | 0x80);
            if (ret) {
                log_error("fail to set reg 0x20eb\n");
                return 1;
            }
            /* erase first sector to force device into ROM mode */
            ret = flash_erase_sector(0);
            if (ret) {
                log_error("flash erase failed\n");
                return 1;
            }
            usleep(300000);
            /* Trigger reset */
            read_reg(0x20e9, &reg);
            write_reg(0x20e9, reg | 0x20);
            write_reg(0x20f8, 0x01);
            write_reg(0x20f5, 0xaa);
            write_reg(0x20f6, 0x55);
            /* Need to ignore error since device is resetting */
            g_ignore_ioctl_error = 1;
            write_reg(0x20f7, 0xa0);
            g_ignore_ioctl_error = 0;
        }
        else {
            log_error("don't know what mode this chip is in 0x%02\n", reg);
            return 1;
        }
        break;
    default:
        log_error("not supported chip 0x%x\n", g_curr_chip);
        return 1;
    }

    return 0;
}

static void print_usage(char *s) {
    fprintf(stderr, "Usage: %s [OPTIONS] UVC_DEV \n", s);
    fprintf(stderr, "  -m  --rom         Enter romboot mode\n"
                    "      NOTE: This is will modify flash content\n");
    fprintf(stderr, "  -e  --erase       Erase flash\n");
    fprintf(stderr, "  -r  --read  file  Read flash data to file\n");
    fprintf(stderr, "  -w  --write file  Write data from file to flash\n\n");

    fprintf(stderr, "Enter romboot mode:\n");
    fprintf(stderr, "    %s -m /dev/video0\n", s);
    fprintf(stderr, "Dump SPI flash:\n");
    fprintf(stderr, "    %s -r spi.bin /dev/video0\n", s);
    fprintf(stderr, "Erase and Write to flash:\n");
    fprintf(stderr, "    %s -e -w spi.bin /dev/video0\n", s);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int i, c;
    int ret = EXIT_SUCCESS;
    int rom_mode = 0, read_mode = 0;
    int write_mode = 0, erase_mode = 0;
    int option_index = 0;
    FILE * fp = NULL;
    size_t fw_sz = 0, fbuf_sz = 0;
    const char* fpath = NULL;
    const char* devpath = NULL;
    uint8_t flashid[3];
    uint8_t u8val;

    static const struct option long_options[] =
    {
        {"rom",   no_argument,       0, 'm'},
        {"erase", no_argument,       0, 'e'},
        {"read",  required_argument, 0, 'r'},
        {"write", required_argument, 0, 'w'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long (argc, argv, "mer:w:",
                        long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'm':
            rom_mode = 1;
            break;
        case 'e':
            erase_mode = 1;
            break;
        case 'w':
            write_mode = 1;
            fpath = optarg;
            break;
        case 'r':
            read_mode = 1;
            fpath = optarg;
            break;
        default:
            printf("gfdgfd %d\n", c);
            print_usage(argv[0]);
        }
    }

    if ((argc-optind) != 1) {
        print_usage(argv[0]);
    }
    devpath = argv[optind];

    /* sanity check */
    if (read_mode && write_mode) {
        log_error("Cannot read and write at same time\n");
        return EXIT_FAILURE;
    }
    if (read_mode && erase_mode) {
        log_error("Cannot read and erase at same time\n");
        return EXIT_FAILURE;
    }

    /* open uvc device */
    g_uvc_fd = open(devpath, O_RDWR | O_NONBLOCK, 0);
    if (g_uvc_fd < 0) {
        log_error("Can't open device\n");
        return EXIT_FAILURE;
    }

    if (write_mode)
    {
        fp = fopen(fpath, "rb");
        if (!fp) {
            log_error("Fail to open '%s'\n", fpath);
            return EXIT_FAILURE;
        }

        fseek(fp, 0L, SEEK_END);
        fw_sz = ftell(fp);
        fseek(fp, 0L, SEEK_SET);

        log_debug("FW bin is: %zd\n", fw_sz);
    }

    if (read_mode)
    {
        fp = fopen(fpath, "wb");
        if (!fp) {
            log_error("Fail to create '%s'\n", fpath);
            return EXIT_FAILURE;
        }
    }

    ret = detect_ic_type(&g_curr_chip);
    if (ret) {
        log_error("fail to detect ic\n");
        ret = EXIT_FAILURE;
        goto out;
    }
    log_info("Detected: %s\n", ic_type_to_str(g_curr_chip));
    if (g_curr_chip != IC_SPCA_2610) {
        log_error("ic not supported\n");
        ret = EXIT_FAILURE;
        goto out;
    }

    if (is_in_rom_mode()) {
        log_info("Chip is in ROM mode\n");
    }

    ret = flash_get_id(flashid, 0x9f);
    if (ret) {
        log_error("get flash id failed\n");
        ret = EXIT_FAILURE;
        goto out;
    }

    log_info("Flash id: %02x %02x %02x\n", flashid[0], flashid[1], flashid[2]);
    const flash_chip_t *fc = flash_get_info(flashid[1], flashid[0], flashid[2]);
    if (!fc) {
        log_error("flash chip is not supported\n");
        ret = EXIT_FAILURE;
        goto out;
    }
    log_info("Flash vendor: %s\n", fc->vendor);
    log_info("Flash part number: %s\n", fc->part);
    log_info("Flash sector size: %u\n", fc->sector_size);
    log_info("Flash sector count: %u\n", fc->sector_count);

    ret = get_isp_status(&u8val);
    if (ret) {
        log_error("get isp status failed\n");
        ret = EXIT_FAILURE;
        goto out;
    }
    if (u8val == 1) {
        printf("ISP is enabled\n");
    }
    else {
        printf("ISP is disabled\n");
    }

    if (rom_mode) {
        log_info("Putting chip into ROM mode...\n");
        ret = reset_to_rom();
        if (ret) {
            printf("Fail to enter rom mode\n");
            ret = EXIT_FAILURE;
            goto out;
        }
        log_info("Done\n");
        ret = EXIT_SUCCESS;
        goto out;
    }

    if (read_mode || write_mode) {
        log_info("Enabling ISP...\n");
        ret = enable_isp();
        if (ret) {
            log_info("Fail to enable ISP\n");
            ret = EXIT_FAILURE;
            goto out;
        }

        log_info("Getting flash buffer size...\n");
        ret = flash_get_buffer_size(&fbuf_sz);
        if (ret) {
            log_error("Fail to get write buffer size\n");
            ret = EXIT_FAILURE;
            goto out;
        }
        log_info("flash buffer size is: %zu\n", fbuf_sz);
    }

    if (read_mode)
    {
        int flashsz;
        uint8_t *rdbuf = (uint8_t*) malloc(fbuf_sz);
        if (!rdbuf) {
            log_error("fail to malloc read buffer\n");
            ret = EXIT_FAILURE;
            goto out;
        }

        flashsz = fc->sector_size * fc->sector_count;
        for (i = 0; i < flashsz ; i+= fbuf_sz)
        {
            log_info("Reading flash at %08x\n", i);
            ret = flash_read(i, fbuf_sz, rdbuf);
            if (ret) {
                log_info("Read flash failed. addr=0x%08x\n", i);
                free(rdbuf);
                ret = EXIT_FAILURE;
                goto out;
            }
            fwrite(rdbuf, fbuf_sz, 1, fp);
        }
        free(rdbuf);
        ret = EXIT_SUCCESS;
        goto out;
    }

    if (erase_mode) {
        uint32_t erase_addr;

        log_info("Begin to erase flash...\n");
        for (i=fc->sector_count-1; i>=0; i--) {
            erase_addr = i * fc->sector_size;
            log_info("Erasing sector %d (addr: 0x%08x)...\n", i, erase_addr);
            ret = flash_erase_sector(erase_addr);
            if (ret) {
                log_error("!!!!Erase Failed!!!!\n");
                ret = EXIT_FAILURE;
                goto out;
            }
        }
        log_info("Erase finished\n");
    }

    if (write_mode) {
        size_t data_left, rlen, rs;
        uint32_t wraddr = 0;
        uint8_t *wrbuf;

        if (!is_in_rom_mode()) {
            log_error("Chip isn't in ROM mode. Please put chip in ROM mode with -m\n");
            ret = EXIT_FAILURE;
            goto out;
        }

        wrbuf = (uint8_t*) malloc(fbuf_sz);
        if (!wrbuf) {
            log_error("fail to malloc write buffer\n");
            ret = EXIT_FAILURE;
            goto out;
        }

        log_info("Writing to flash....\n");

        data_left = fw_sz;
        while (data_left) {
            rlen = (data_left < fbuf_sz) ? data_left : fbuf_sz;
            rs = fread(&wrbuf[0], 1, rlen, fp);
            if (rlen != rs) {
                log_error("read failure\n");
                ret = EXIT_FAILURE;
                free(wrbuf);
                goto out;
            }

            log_info("Writing to flash at 0x%08x, len %zu....\n", wraddr, rs);
            ret = flash_write(wraddr, rs, wrbuf);
            if (ret) {
                log_info("Write to flash failed\n");
                ret = EXIT_FAILURE;
                free(wrbuf);
                goto out;
            }
            wraddr += rs;
            data_left -= rs;
        }
        free(wrbuf);

        log_info("Finish write to flash\n");
    }


    ret = EXIT_SUCCESS;
out:
    if (fp)
        fclose(fp);
    close(g_uvc_fd);
    return ret;
}
