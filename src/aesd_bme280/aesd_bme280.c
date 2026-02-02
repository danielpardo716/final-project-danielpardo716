#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/types.h>
#include "aesd_bme280.h"

static uint32_t bme280_measurement_period;

static BME280_INTF_RET_TYPE bme280_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int result;
    struct aesd_bme280_dev* dev = (struct aesd_bme280_dev*)intf_ptr;
    struct i2c_client* client = dev->i2c_client;

    // First write the register address, then read the data
    if ((result = i2c_smbus_write_byte(client, reg_addr)) < 0)
    {
        return result;
    }

    return i2c_master_recv(client, reg_data, length);
}

static BME280_INTF_RET_TYPE bme280_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int result;
    struct aesd_bme280_dev* dev = (struct aesd_bme280_dev*)intf_ptr;
    struct i2c_client* client = dev->i2c_client;
    uint8_t buffer[33]; // Bosch limits max write length to 32 bytes + 1 byte for reg_addr

    if (length > 32)
    {
        return -EINVAL; // Invalid length
    }

    buffer[0] = reg_addr;
    memcpy(&buffer[1], reg_data, length);
    return i2c_master_send(client, buffer, length + 1);
}

static void bme280_delay_us(uint32_t period, void *intf_ptr)
{
    // udelay is safe for delays up to ~10 ms; for longer periods use usleep_range
    if (period <= 10000)
    {
        udelay(period);
    }
    else
    {
        usleep_range(period, period + 50);
    }
}

static void bme280_error_codes_print_result(const char api_name[], int8_t rslt)
{
    printk("%s\t", api_name);

    switch (rslt)
    {
        case BME280_E_NULL_PTR:
            printk("Error [%d] : Null pointer error.", rslt);
            printk(
                "It occurs when the user tries to assign value (not address) to a pointer, which has been initialized to NULL.\r\n");
            break;

        case BME280_E_COMM_FAIL:
            printk("Error [%d] : Communication failure error.", rslt);
            printk(
                "It occurs due to read/write operation failure and also due to power failure during communication\r\n");
            break;

        case BME280_E_DEV_NOT_FOUND:
            printk("Error [%d] : Device not found error. It occurs when the device chip id is incorrectly read\r\n",
                    rslt);
            break;

        case BME280_E_INVALID_LEN:
            printk("Error [%d] : Invalid length error. It occurs when write is done with invalid length\r\n", rslt);
            break;

        default:
            printk("Error [%d] : Unknown error code\r\n", rslt);
            break;
    }
}

static int aesd_open(struct inode* inode, struct file* filp)
{
    PDEBUG("aesd_bme280 opened");
    struct aesd_bme280_dev* dev = container_of(inode->i_cdev, struct aesd_bme280_dev, cdev);
    filp->private_data = dev;
    return 0;
}

static int aesd_release(struct inode* inode, struct file* filp)
{
    PDEBUG("aesd_bme280 released");
    return 0;
}

static int aesd_read(struct file* filp, char __user* buf, size_t count, loff_t* ppos)
{
    struct aesd_bme280_dev* dev = filp->private_data;
    struct bme280_dev* sensor = &dev->sensor;
    struct bme280_data comp_data;
    char buffer[64];
    int len, result;

    if ((result = bme280_get_sensor_data(BME280_ALL, &comp_data, sensor)) != 0)
    {
        bme280_error_codes_print_result("bme280_get_sensor_data", result);
        return result;
    }

    len = scnprintk(buffer, sizeof(buffer),
                    "T=%.2f°C P=%.2fhPa H=%.2f%%\n",
                    comp_data.temperature,
                    comp_data.pressure / 100.0,
                    comp_data.humidity);
    if (*ppos > len)
    {
        // No more data to read (EOF)
        return 0;
    }

    if (count > (len - *ppos))
    {
        // Adjust count to read only available data
        count = len - *ppos;
    }

    if (copy_to_user(buf, buffer + *ppos, count))
    {
        return -EFAULT;
    }

    *ppos += count;
    return count;
}

static struct file_operations fops = {
    .owner =            THIS_MODULE,
    .read =             aesd_read,
    .open =             aesd_open,
    .release =          aesd_release,
};

static int aesd_bme280_i2c_probe(struct i2c_client* client, const struct i2c_device_id* id)
{
    struct aesd_bme280_dev* dev;
    int result;

    // Allocate memory for device structure
    dev = devm_kzalloc(&client->dev, sizeof(struct aesd_bme280_dev), GFP_KERNEL);
    if (!dev)
    {
        PDEBUG("Failed to allocate memory for device structure\n");
        return -ENOMEM;
    }
    dev->i2c_client = client;
    i2c_set_clientdata(client, dev);

    // Configure sensor to use our I2C methods
    dev->sensor.intf        = BME280_I2C_INTF;
    dev->sensor.intf_ptr    = dev;                      // Store pointer to be used in read/write functions
    dev->sensor.read        = bme280_i2c_read;
    dev->sensor.write       = bme280_i2c_write;
    dev->sensor.delay_us    = bme280_delay_us;

    // Initialize sensor
    int result = 0;
    if ((result = bme280_init(dev->sensor)) < 0)
    {
        bme280_error_codes_print_result("bme280_init", result);
        return result;
    }

    // Configure sensor settings
    struct bme280_settings settings;
    if ((result = bme280_get_sensor_settings(&settings, dev->sensor)) != 0)
    {
        bme280_error_codes_print_result("bme280_get_sensor_settings", result);
        return result;
    }
    settings.filter = BME280_FILTER_COEFF_2;                // Filter coefficient
    settings.osr_h = BME280_OVERSAMPLING_1X;                // Oversampling for humidity
    settings.osr_p = BME280_OVERSAMPLING_1X;                // Oversampling for pressure
    settings.osr_t = BME280_OVERSAMPLING_1X;                // Oversampling for temperature
    settings.standby_time = BME280_STANDBY_TIME_0_5_MS;     // Standby time
    if ((result = bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, dev->sensor)) != 0)
    {
        bme280_error_codes_print_result("bme280_set_sensor_settings", result);
        return result;
    }

    // Always set the power mode after setting the configuration
    if ((result = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, dev->sensor)) != 0)
    {
        bme280_error_codes_print_result("bme280_set_sensor_mode", result);
        return result;
    }

    // Calculate measurement time in microseconds
    if ((result = bme280_cal_meas_delay(&bme280_measurement_period, &settings)) != 0)
    {
        bme280_error_codes_print_result("bme280_cal_meas_delay", result);
        return result;
    }
    if ((result = init_sensor(&device.sensor)) != 0)
    {
        PDEBUG("Failed to initialize BME280 sensor\n");
        unregister_chrdev_region(dev, 1);
        return result;
    }

    // Allocate character device region
    if ((result = alloc_chrdev_region(&dev->devno, 0, 1, "aesd_bme280")) < 0)
    {
        PDEBUG("Failed to allocate char device region\n");
        return result;
    }

    // Setup character device
    cdev_init(&dev->cdev, &fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &fops;
    if (cdev_add(&dev->cdev, dev->devno, 1) != 0)
    {
        PDEBUG("Failed to set up character device\n");
        unregister_chrdev_region(dev->devno, 1);
        return -ENODEV;
    }

    // Create device file in /dev
    device_create(class_create(THIS_MODULE, "aesd_bme280"), NULL, dev->devno, NULL, "aesd_bme280");
    dev_info(&client->dev, "aesd_bme280 device registered with major %d and minor %d\n", MAJOR(dev->devno), MINOR(dev->devno));

    return 0;
}

static int aesd_bme280_i2c_remove(struct i2c_client* client)
{
    struct aesd_bme280_dev* dev = i2c_get_clientdata(client);

    device_destroy(class_find("aesd_bme280"), dev->devno);
    cdev_del(&dev->cdev);
    unregister_chrdev_region(dev->devno, 1);
    return 0;
}

/* I²C device ID table – matches the DT compatible string */
static const struct i2c_device_id bme280_id[] = {
    { "aesd_bme280", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, bme280_id);

/* OF match table for Device Tree */
static const struct of_device_id bme280_of_match[] = {
    { .compatible = "bosch,bme280" },
    { }
};
MODULE_DEVICE_TABLE(of, bme280_of_match);


/* I²C driver definition */
static struct i2c_driver aesd_bme280_i2c_driver = {
    .driver = {
        .name   = "aesd_bme280",
        .of_match_table = bme280_of_match,
        .owner = THIS_MODULE,
    },
    .probe    = aesd_bme280_i2c_probe,
    .remove   = aesd_bme280_i2c_remove,
    .id_table = bme280_id,
};

module_i2c_driver(aesd_bme280_i2c_driver);
MODULE_AUTHOR("Daniel Pardo");
MODULE_LICENSE("Dual BSD/GPL");