KAS_BUILD = uv run kas build
KAS_DIR = ./kas
KAS_BASE = $(KAS_DIR)/base.yml

.PHONY: all rpi0 qemu clean

# Build both using multi-machine config
all:
	$(KAS_BUILD) $(KAS_BASE):$(KAS_DIR)/kas-all.yml

# Build Raspberry Pi Zero W image
raspberrypi0-wifi:
	$(KAS_BUILD) $(KAS_BASE):$(KAS_DIR)/kas-raspberrypi0-wifi.yml

# Build QEMU image
qemu:
	$(KAS_BUILD) $(KAS_BASE):$(KAS_DIR)/kas-qemu.yml

# Clean Yocto build directories
clean:
	rm -rf build tmp sstate-cache downloads