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
.PHONY: build flash qemu config ssh

build:
	@$(SCRIPT_DIR)/build.sh

qemu:
	@$(SCRIPT_DIR)/qemu.sh

flash:
	@$(SCRIPT_DIR)/flash.sh

config:
	@$(SCRIPT_DIR)/save-config.sh

ssh:
	@$(SCRIPT_DIR)/ssh-connect.sh