# AESD Final Project
For a detailed project overview, see [Wiki/Project Overview](https://github.com/danielpardo716/final-project-danielpardo716/wiki/Project-Overview).

For the schedule, see [Wiki/Schedule](https://github.com/danielpardo716/final-project-danielpardo716/wiki/Schedule).

## Requirements
- Ubuntu 24.04+ or another Linux host with Yocto dependencies installed
- Python 3.8+
- [uv](https://docs.astral.sh/uv/)
- At least 100 GB free disk space for full builds

## Building the Project
To build this project, the [kas](https://github.com/siemens/kas) tool is used to simplify configuration and create an easily reproducible build across host machines.

To build using kas, run:
```bash
make <target>
```
where <target> is qemu, raspberrypi0-wifi, all, or clean.

### Flashing to Target
To flash to an SD card (**Note**: replace /dev/sdX with your SD card device):
```bash
sudo dd if=build/tmp/deploy/images/raspberrypi0-wifi/core-image-base-raspberrypi0-wifi.rpi-sdimg of=/dev/sdX bs=4M status=progress conv=fsync
```

Insert SD card into Raspberry Pi Zero W to boot the new image.