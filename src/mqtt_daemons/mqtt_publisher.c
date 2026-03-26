#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

#include "mqtt_common.h"
#include "../aesd_bme280/aesd_bme280_ioctl.h"

#define SENSOR_DRIVER_PATH  "/dev/aesd_bme280"

static struct mosquitto* mqtt_client = NULL;

static int sensor_fd = -1;

static void cleanup_and_exit(int exit_code)
{
    mqtt_close(mqtt_client);
    if (sensor_fd >= 0)
    {
        close(sensor_fd);
    }
    closelog();
    exit(exit_code);
}

static void signal_handler(int signal_number)
{
    if ((signal_number == SIGINT) || (signal_number == SIGTERM))
    {
        syslog(LOG_INFO, "Caught signal %d, exiting", signal_number);
        cleanup_and_exit(EXIT_SUCCESS);
    }
}

static void init_signal_handler()
{
    struct sigaction signal = { .sa_handler = signal_handler };
    if (sigaction(SIGINT, &signal, NULL) != 0)
    {
        syslog(LOG_ERR, "Unable to register SIGINT");
        cleanup_and_exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &signal, NULL) != 0)
    {
        syslog(LOG_ERR, "Unable to register SIGTERM");
        cleanup_and_exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
    int result;    
    struct aesd_bme280_data sensor_data;
    char mqtt_message[128];

    openlog(NULL, 0, LOG_USER);
    init_signal_handler();

    mqtt_init(mqtt_client, cleanup_and_exit, NULL);

    // Open sensor
    sensor_fd = open(SENSOR_DRIVER_PATH, O_RDONLY);
    if (sensor_fd < 0)
    {
        syslog(LOG_ERR, "Failed to open device %s: %s\n", SENSOR_DRIVER_PATH, strerror(errno));
        result = errno;
        cleanup_and_exit(result);
    }
    
    // Daemonize the process
    if (daemon(0, 0) < 0)
    {
        syslog(LOG_ERR, "Failed to create daemon: %s", strerror(errno));
        cleanup_and_exit(EXIT_FAILURE);
    }

    mqtt_wait_for_connection(mqtt_client, cleanup_and_exit);

    while (1)
    {
        // Read sensor values
        if ((result = ioctl(sensor_fd, AESD_BME280_GET_ALL, &sensor_data)) < 0)
        {
            syslog(LOG_ERR, "Failed to get sensor data: %s\n", strerror(result));
        }
        else
        {
            // Format and publish message
            sprintf(mqtt_message, "Temperature: %d.%02d°C, Pressure: %u.%02uPa, Humidity: %u.%01u%%",
                sensor_data.temperature / 100,  abs(sensor_data.temperature) % 100,
                sensor_data.pressure / 256,     (sensor_data.pressure % 256) * 100 / 256,
                sensor_data.humidity / 1024,    (sensor_data.humidity % 1024) * 10 / 1024
            );

            if ((result = mosquitto_publish(mqtt_client, NULL, MQTT_TOPIC, sizeof(mqtt_message), mqtt_message, 0, false)) != MOSQ_ERR_SUCCESS)
            {
                syslog(LOG_ERR, "Failed to publish message to topic: %s", mosquitto_strerror(result));
            }
        }

        // Sleep for 3 seconds
        sleep(3);
    }

    return 0;
}