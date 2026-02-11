#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "../../aesd_bme280/aesd_bme280_ioctl.h"

#define DRIVER_PATH "/dev/aesd_bme280"

enum Options
{
    OPTION_TEMPERATURE = 't',
    OPTION_PRESSURE = 'p',
    OPTION_HUMIDITY = 'h',
    OPTION_ALL = 'a',
    OPTION_HELP = '?'
};

static void print_usage(const char *progname)
{
    fprintf(stderr,
        "Usage: %s [options]\n"
        "\nOptions:\n"
        "  -t, --temp        Print temperature only\n"
        "  -p, --press       Print pressure only\n"
        "  -h, --hum         Print humidity only\n"
        "  -a, --all         Print all three measurements (single ioctl)\n"
        "  -?, --help        Show this help message\n",
        progname);
}

static const struct option long_opts[] = {
    {"temp",  no_argument, NULL, 't'},
    {"press", no_argument, NULL, 'p'},
    {"hum",   no_argument, NULL, 'h'},
    {"all",   no_argument, NULL, 'a'},
    {"help",  no_argument, NULL, '?'},
    {0, 0, 0, 0}
};

int main(int argc, char* argv[])
{
    int opt;
    long result = 0;
    enum Options selected_option = OPTION_HELP;

    while ((opt = getopt_long(argc, argv, "tph?a", long_opts, NULL)) != -1) 
    {
        switch (opt)
        {
            case 't': selected_option = OPTION_TEMPERATURE; break;
            case 'p': selected_option = OPTION_PRESSURE; break;
            case 'h': selected_option = OPTION_HUMIDITY; break;
            case 'a': selected_option = OPTION_ALL; break;
            case '?':
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    /* If nothing was selected, show usage and exit */
    if (selected_option == OPTION_HELP) 
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(DRIVER_PATH, O_RDONLY);
    if (fd < 0)
    {
        printf("Failed to open device %s: %s\n", DRIVER_PATH, strerror(errno));
        result = errno;
        goto cleanup;
    }

    int value;
    switch (selected_option)
    {
        case OPTION_TEMPERATURE:
        {
            if ((result = ioctl(fd, AESD_BME280_GET_TEMPERATURE, &value)) < 0)
            {
                printf("Failed to get temperature: %s\n", strerror(result));
            }
            else
            {
                printf("Temperature: %d.%02d°C\n", value / 100, abs(value) % 100);
            }
            break;
        }

        case OPTION_PRESSURE:
        {
            if ((result = ioctl(fd, AESD_BME280_GET_PRESSURE, &value)) < 0)
            {
                printf("Failed to get pressure: %s\n", strerror(result));
            }
            else
            {
                printf("Pressure: %u.%02uPa\n", value / 256, (value % 256) * 100 / 256);
            }
            break;
        }

        case OPTION_HUMIDITY:
        {
            if ((result = ioctl(fd, AESD_BME280_GET_HUMIDITY, &value)) < 0)
            {
                printf("Failed to get humidity: %s\n", strerror(result));
            }
            else
            {
                printf("Humidity: %u.%01u%%\n", value / 1024, (value % 1024) * 10 / 1024);
            }
            break;
        }

        case OPTION_ALL:
        {
            struct aesd_bme280_data data;
            if ((result = ioctl(fd, AESD_BME280_GET_ALL, &data)) < 0)
            {
                printf("Failed to get all sensor data: %s\n", strerror(result));
            }
            else
            {
                printf("All Sensor Data:\n");
                printf("  Temperature: %d.%02d°C\n", data.temperature / 100, abs(data.temperature) % 100);
                printf("  Pressure: %u.%02uPa\n", data.pressure / 256, (data.pressure % 256) * 100 / 256);
                printf("  Humidity: %u.%01u%%\n", data.humidity / 1024, (data.humidity % 1024) * 10 / 1024);
            }
            break;
        }

        default:
            // This should never be reached due to earlier checks
            result = -EINVAL;
            break;
    }

cleanup:
    close(fd);
    return result;
}