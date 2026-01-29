
##############################################################
#
# AESD-FINAL-PROJECT
#
##############################################################

AESD_FINAL_PROJECT_VERSION = 1.0
AESD_FINAL_PROJECT_SITE = $(TOPDIR)/../src
AESD_FINAL_PROJECT_SITE_METHOD = local

AESD_FINAL_PROJECT_DEPENDENCIES = libgpiod
AESD_FINAL_PROJECT_CONF_OPTS   = $(TARGET_CFLAGS) $(TARGET_LDFLAGS)

AESD_ASSIGNMENTS_MODULE_SUBDIRS = aesd_bme280
AESD_ASSIGNMENTS_MODULE_MAKE_OPTS = KVERSION=$(LINUX_VERSION_PROBED)

define AESD_FINAL_PROJECT_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/led_blinky all
endef

define AESD_FINAL_PROJECT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/led_blinky/led_blinky $(TARGET_DIR)/bin
endef

$(eval $(kernel-module))
$(eval $(generic-package))
