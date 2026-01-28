#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

#define GPIO_CHIP_PATH  "/dev/gpiochip0"
#define GPIO_PIN_MAX    32
#define GPIO_PIN_MIN    0

static struct gpiod_chip* gpio_chip;
static struct gpiod_line* gpio_line;

static void cleanup_and_exit(int exit_code)
{
    closelog();
    if (gpio_line != NULL)
    {
        gpiod_line_release(gpio_line);
    }
    if (gpio_chip != NULL)
    {
        gpiod_chip_close(gpio_chip);
    }
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

int main(int argc, char** argv)
{
    openlog(NULL, 0, LOG_USER);
    init_signal_handler();

    // Parse input into GPIO pin number
    if (argc != 2)
    {
        syslog(LOG_ERR, "Must provide a GPIO number.");
        cleanup_and_exit(EXIT_FAILURE);
    }
    char* endptr;
    int gpio_pin = (int)strtol(argv[1], &endptr, 10);
    if ((endptr == argv[1]) ||                                  // No digits
        (*endptr != '\0') ||                                    // Leftover parts of the string
        (gpio_pin > GPIO_PIN_MAX) || (gpio_pin < GPIO_PIN_MIN)) // Number out of range
    {
        syslog(LOG_ERR, "Invalid pin number entered");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // Obtain GPIO pin
    if ((gpio_chip = gpiod_chip_open(GPIO_CHIP_PATH)) == NULL)
    {
        syslog(LOG_ERR, "Failed to open GPIO chip 0.");
        cleanup_and_exit(EXIT_FAILURE);
    }
    if ((gpio_line = gpiod_chip_get_line(gpio_chip, gpio_pin)) == NULL)
    {
        syslog(LOG_ERR, "Failed to get GPIO %d.", gpio_pin);
        cleanup_and_exit(EXIT_FAILURE);
    }
    if (gpiod_line_request_output(gpio_line, "aesdled", 1) != 0)
    {
        syslog(LOG_ERR, "GPIO line output request failed.");
        cleanup_and_exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "GPIO %d obtained, starting blinky.", gpio_pin);
    int gpio_value = 0;

    while (1)
    {
        gpiod_line_set_value(gpio_line, gpio_value);
        syslog(LOG_INFO, "GPIO %d set to %d", gpio_pin, gpio_value);
        sleep(1);
        gpio_value = (gpio_value) ? 0 : 1;
    }
}