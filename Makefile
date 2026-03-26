SHELL := /usr/bin/env bash
.SHELLFLAGS := -eu -o pipefail -c

# Directory that holds the helpers
SCRIPT_DIR := $(CURDIR)/scripts

# -----------------------------------------------------------------
# Convenience: allow `make build` to be typed as just `make`
# -----------------------------------------------------------------
.PHONY: all
all: build

# -----------------------------------------------------------------
# Individual targets – just invoke the Bash helpers
# -----------------------------------------------------------------
.PHONY: build flash qemu config ssh keygen mosquitto dirclean menuconfig

build:
	@$(SCRIPT_DIR)/build.sh

qemu:
	@$(SCRIPT_DIR)/qemu.sh

flash:
	@$(SCRIPT_DIR)/flash.sh

config:
	@$(SCRIPT_DIR)/save-config.sh

ssh:
	@$(SCRIPT_DIR)/ssh-connect.sh $(HOSTNAME)

keygen:
	@$(SCRIPT_DIR)/keygen.sh

mosquitto:
	mosquitto -c /etc/mosquitto/mosquitto.conf -d

dirclean:
	cd buildroot && make aesd-final-project-dirclean && cd ..

menuconfig:
	cd buildroot && make menuconfig && cd ..