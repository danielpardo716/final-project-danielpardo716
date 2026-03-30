#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <gpiod.h>
#include "mqtt_common.h"

#define MQTT_USERNAME       "aesd_mqtt_subscriber"
#define LED_GPIO_CHIP_PATH  "/dev/gpiochip0"
#define LED_GPIO_PIN        26

static struct mosquitto* mqtt_client = NULL;
static struct gpiod_chip* led_gpio_chip;
static struct gpiod_line* led_gpio_line;

static void cleanup_and_exit(int exit_code)
{
    mqtt_close(&mqtt_client);
    if (led_gpio_line != NULL)
    {
        gpiod_line_release(led_gpio_line);
    }
    if (led_gpio_chip != NULL)
    {
        gpiod_chip_close(led_gpio_chip);
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

static void mqtt_message_callback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* message)
{
    if (message->payloadlen > 0)
    {
        syslog(LOG_INFO, "Received MQTT message on topic %s: %s", message->topic, (char*)message->payload);

        // Toggle LED state
        int current_value = gpiod_line_get_value(led_gpio_line);
        if (current_value < 0)
        {
            syslog(LOG_ERR, "Failed to read GPIO line value.");
            return;
        }
        gpiod_line_set_value(led_gpio_line, !current_value);
    }
}

int main(int argc, char* argv[])
{
    int result;    
    openlog(NULL, 0, LOG_USER);
    init_signal_handler();

    // Obtain GPIO pin
    if ((led_gpio_chip = gpiod_chip_open(LED_GPIO_CHIP_PATH)) == NULL)
    {
        syslog(LOG_ERR, "Failed to open GPIO chip 0.");
        cleanup_and_exit(EXIT_FAILURE);
    }
    if ((led_gpio_line = gpiod_chip_get_line(led_gpio_chip, LED_GPIO_PIN)) == NULL)
    {
        syslog(LOG_ERR, "Failed to get GPIO %d.", LED_GPIO_PIN);
        cleanup_and_exit(EXIT_FAILURE);
    }
    if (gpiod_line_request_output(led_gpio_line, "aesdled", 1) != 0)
    {
        syslog(LOG_ERR, "GPIO line output request failed.");
        cleanup_and_exit(EXIT_FAILURE);
    }

    mqtt_init(&mqtt_client, MQTT_USERNAME, cleanup_and_exit, mqtt_message_callback);

    // Daemonize the process
    if (daemon(0, 0) < 0)
    {
        syslog(LOG_ERR, "Failed to create daemon: %s", strerror(errno));
        cleanup_and_exit(EXIT_FAILURE);
    }

    mqtt_wait_for_connection(mqtt_client, cleanup_and_exit);

    // Subscribe to the topic
    if ((result = mosquitto_subscribe(mqtt_client, NULL, MQTT_TOPIC_TEMP, 0)) != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to subscribe to topic "MQTT_TOPIC_TEMP": %s", mosquitto_strerror(result));
        cleanup_and_exit(result);
    }
    
    while (1);
    return 0;
}