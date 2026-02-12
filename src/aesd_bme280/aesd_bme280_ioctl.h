#ifndef AESD_BME280_IOCTL_H_
#define AESD_BME280_IOCTL_H_

#include <linux/ioctl.h>

struct aesd_bme280_data
{
     int temperature;    ///< Temperature in milli-degrees Celsius
     int pressure;       ///< Pressure in Pascals
     int humidity;       ///< Humidity in milli-percent relative humidity
};

#define AESD_BME280_IOCTL_MAGIC         'b'
#define AESD_BME280_GET_TEMPERATURE     _IOR(AESD_BME280_IOCTL_MAGIC, 0, int)
#define AESD_BME280_GET_PRESSURE        _IOR(AESD_BME280_IOCTL_MAGIC, 1, int)
#define AESD_BME280_GET_HUMIDITY        _IOR(AESD_BME280_IOCTL_MAGIC, 2, int)
#define AESD_BME280_GET_ALL             _IOR(AESD_BME280_IOCTL_MAGIC, 3, struct aesd_bme280_data)

#endif