
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

define AESD_FINAL_PROJECT_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
endef

define AESD_FINAL_PROJECT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/led_blinky $(TARGET_DIR)/bin
endef

$(eval $(generic-package))
