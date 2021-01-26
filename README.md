# VXIS USB3 HDMI Capture Firmware Mod

Low-cost USB3 HDMI capture cards with VXIS chipset usually come with a sharpening filter enabled. The sharpening filter is fine for movies and video games but not what you would want for screen capturing. Unfortunately, the manufacture didn't expose any interface to turn this off. Therefore, below will detail how I had modified and managed to get rid of the filter.

**DISCLAIMER:** This information is provided 'as is' without warranty of any kind. Use it at your own risk. We can't be held accountable for broken devices or things that went wrong.

## Hardware

<img src="images/device.jpg" width="200">

This particular device is branded "Wiistar USB3.0 HD Capture" which currently retails for around $40 on [AliExpress](https://www.aliexpress.com/item/4001044872081.html) and the same one for $55 on [Amazon](https://www.amazon.com/gp/product/B083HWLHF5) with faster shipping if you are in the US. From the advertised specification, it can output 1080p@60Hz in YUY2. Note the USB-C port only work in one orientation for SuperSpeed, the other orientation will show up as a USB2 device and is limited to MJPEG only.

v4l2 show the following supported format:
```
Driver Info:
        Driver name      : uvcvideo
        Card type        : FHD Capture: FHD Capture
        Bus info         : usb-0000:2a:00.3-3
        Driver version   : 5.10.6
        Capabilities     : 0x84a00001
                Video Capture
                Metadata Capture
                Streaming
                Extended Pix Format
                Device Capabilities
        Device Caps      : 0x04200001
                Video Capture
                Streaming
                Extended Pix Format
ioctl: VIDIOC_ENUM_FMT
        Type: Video Capture

        [0]: 'MJPG' (Motion-JPEG, compressed)
                Size: Discrete 1920x1080
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 640x480
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 800x600
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1024x768
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x720
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x960
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x1024
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1360x768
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1400x900
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1440x900
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
        [1]: 'YUYV' (YUYV 4:2:2)
                Size: Discrete 1920x1080
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 640x480
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 800x600
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1024x768
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x720
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x960
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1280x1024
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1360x768
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1400x900
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 1440x900
                        Interval: Discrete 0.017s (60.000 fps)
                        Interval: Discrete 0.033s (30.000 fps)
```
### Top view

<img src="images/top.jpg" width="600">

### Bottom view

<img src="images/bottom.jpg" width="600">

### Block Diagram

![block diagram picture](images/block_diagram.png)

### Disable Sharpening

Sharpening is done inside the VS2828 USB chip. To disable, set bit 7 high for register 0x2170 by using the [xdata](flasher/xdata.c) tool.

There is also black level adjustment done by VS9989 video processor. To disable, clear bit 7 for register 0x35 by using the [i2c_vs9989](flasher/i2c_vs9989.c) tool.

Details of the registers is avaliable on the wiki page.

Setting that we found to work best:
```
# Disable black point adjustment
./i2c_vs9989 -w 0x38 0 /dev/v4l/vxis

# disable sharpness/contrast/color processing
./xdata -w 0x2170 0x80 /dev/v4l/vxis

# Set good ramps
v4l2-ctl -d /dev/v4l/vxis -c brightness=143
v4l2-ctl -d /dev/v4l/vxis -c contrast=110
v4l2-ctl -d /dev/v4l/vxis -c saturation=148

# Fix chroma siting
./i2c_vs9989 -w 0x35 0x28 /dev/v4l/vxis
./i2c_vs9989 -w 0x8d 0x48 /dev/v4l/vxis
```
Thanks to @marcan for finding these settings!

### Custom Firmware

The goal of this project is to make a custom firmware that replaces the original with more controls and exposes all functionalities of the hardware. But we need to understand the hardware enough before we could start writing the firmware. I will keep update the wiki with my findings and hopefully write a custom firmware for this device. If you want to try to explore the hardware your self, there are some example codes available in the firmware folder that you can play with :)
