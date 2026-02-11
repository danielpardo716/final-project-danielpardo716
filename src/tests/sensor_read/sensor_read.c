#include <stddef.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DRIVER_PATH "/dev/aesd_bme280"

int main(int argc, char* argv[])
{
    int result = 0;
    openlog(NULL, 0, LOG_USER);

    int fd = open(DRIVER_PATH, O_RDONLY);
    if (fd < 0)
    {
        syslog(LOG_ERR, "Failed to open device %s: %s", DRIVER_PATH, strerror(errno));
        result = errno;
        goto cleanup;
    }

    char buffer[128];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0)
    {
        syslog(LOG_ERR, "Failed to read from device %s: %s", DRIVER_PATH, strerror(errno));
        close(fd);
        result = errno;
        goto cleanup;
    }

    buffer[bytes_read] = '\0'; // Null-terminate the buffer
    printf("BME280 Sensor Data: %s\n", buffer);

cleanup:
    close(fd);
    closelog();
    return result;
}