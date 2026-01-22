KAS_CMD = uv run kas
KAS_BUILD = $(KAS_CMD) build
KAS_SHELL = $(KAS_CMD) shell
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

# Enter shell to perform bitbake operations manually
shell:
	$(KAS_SHELL) $(KAS_BASE)

# Clean Yocto build directories
clean:
	rm -rf build tmp sstate-cache downloads
