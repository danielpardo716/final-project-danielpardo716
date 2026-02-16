
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

AESD_FINAL_PROJECT_MODULE_SUBDIRS = aesd_bme280
AESD_FINAL_PROJECT_MODULE_MAKE_OPTS = KVERSION=$(LINUX_VERSION_PROBED)

define AESD_FINAL_PROJECT_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/tests/led_blinky all
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/tests/mqtt_publish all
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/tests/sensor_read all
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/mqtt_daemons all
endef

define AESD_FINAL_PROJECT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/tests/led_blinky/led_blinky $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/tests/mqtt_publish/mqtt_publish $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/tests/sensor_read/sensor_read $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/mqtt_daemons/mqtt_publisher $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/mqtt_daemons/mqtt_subscriber $(TARGET_DIR)/bin
endef

$(eval $(kernel-module))
$(eval $(generic-package))
