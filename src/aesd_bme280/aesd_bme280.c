#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include <linux/i2c.h>
#include "aesd_bme280.h"

MODULE_AUTHOR("Daniel Pardo");
MODULE_LICENSE("Dual BSD/GPL");

static struct aesd_bme280_dev device;              ///< Device structure for the BME280 sensor.

int aesd_open(struct inode* inode, struct file* filp)
{
    PDEBUG("aesd_bme280 opened");
    struct aesd_bme280_dev* dev;
    dev = container_of(inode->i_cdev, struct aesd_bme280_dev, cdev);
    filp->private_data = dev;
    return 0;
}

int aesd_release(struct inode* inode, struct file* filp)
{
    PDEBUG("aesd_bme280: released");
    return 0;
}

int aesd_read(struct file* filp, char __user* buf, size_t count, loff_t* ppos)
{
    struct bme280* sensor = filp->private_data->sensor;

    ssize_t num_bytes_read = MEASUREMENT_LEN;
    ssize_t retval = 0;
    int rmw_val;
    char measurements[MEASUREMENT_LEN];

    // Force a reading
    rmw_val = i2c_smbus_read_byte_data(bme280_device.bme280_i2c_client, BME280_CTRL_MEAS_REG_ADDR);
    retval = i2c_smbus_write_byte_data(bme280_device.bme280_i2c_client, BME280_CTRL_MEAS_REG_ADDR, (rmw_val | (1 << MODE_LSB)));

    if (retval < 0)
    {
        printk(KERN_ERR "Coudn't write data to force reading Result = %ld\n", retval);
        return -1;
    }

    // Wait while a measurement is in progress
    while(!(i2c_smbus_read_byte_data(bme280_device.bme280_i2c_client, BME280_STATUS_REG_ADDR) & MEASUREMENT_IN_PROGRESS));
    temperature_val = bme280_read_temperature(sensor);

    // Read Pressure value
    pressure_val = bme280_read_pressure(sensor);
    if(pressure_val == -1)
    {
        printk(KERN_ERR "Couldn't read pressure value. Error = %ld\n", pressure_val);
        return -1;
    }

    // Copy the entry from the specified offset to the user-provided buffer
    snprintf(measurements, sizeof(measurements), "%ld %lu", temperature_val, pressure_val);
    if(copy_to_user(buf, measurements, MEASUREMENT_LEN))
    {
        return -EFAULT;
    }

    PDEBUG("Number of bytes read = %ld\n", num_bytes_read);
    return num_bytes_read;
}

struct file_operations fops = {
    .owner =            THIS_MODULE,
    .read =             aesd_read,
    .open =             aesd_open,
    .release =          aesd_release,
};

int aesd_init_module(void)
{
    dev_t dev = 0;
    int result = 0;

    // Allocate character device region
    if ((result = alloc_chrdev_region(&dev, device.minor, 1, "aesd_bme280")) < 0)
    {
        PDEBUG("Failed to allocate char device region\n");
        return result;
    }
    device.major = MAJOR(dev);

    // Setup character device
    int err, devno = MKDEV(device.major, device.minor);
    cdev_init(&device.cdev, &fops);
    device.cdev.owner = THIS_MODULE;
    device.cdev.ops = &fops;
    if (cdev_add(&device.cdev, devno, 1) != 0)
    {
        PDEBUG("Failed to set up character device\n");
        unregister_chrdev_region(dev, 1);
        return -ENODEV;
    }

    // Initialize BME280 sensor
    if ((result = bme280_init(&device.sensor)) < 0)
    {
        PDEBUG("Failed to initialize BME280 sensor\n");
        unregister_chrdev_region(dev, 1);
        return result;
    }

    return result;
}

void aesd_cleanup_module(void)
{
    // Cleanup I2C peripheral
    bme280_cleanup(&device.sensor);

    // Cleanup character device
    dev_t dev = MKDEV(device.major, device.minor);
    cdev_del(&device.cdev);
    unregister_chrdev_region(dev, 1);
}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);