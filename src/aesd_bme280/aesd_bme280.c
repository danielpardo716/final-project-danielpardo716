#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include "aesd-bme280.h"

MODULE_AUTHOR("Daniel Pardo");
MODULE_LICENSE("Dual BSD/GPL");

int aesd_init_module(void)
{

}

void aesd_cleanup_module(void)
{

}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);