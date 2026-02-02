#ifndef AESD_BME280_H_
#define AESD_BME280_H_

#include <linux/i2c.h>
#include "bme280_api/bme280.h"

#define AESD_DEBUG 1  //Remove comment on this line to enable debug

#undef PDEBUG             /* undef it, just in case */
#ifdef AESD_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "aesd_bme280: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

struct aesd_bme280_dev 
{
     dev_t devno;
     struct cdev cdev;
     struct bme280_dev sensor;
     struct i2c_client* i2c_client;
};

#endif /* AESD_BME280_H_ */