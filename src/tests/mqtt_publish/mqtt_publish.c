#include <syslog.h>
#include "mosquitto.h"

#define MQTT_BROKER_ADDR    "localhost"
#define MQTT_BROKER_PORT    1883
#define MQTT_TOPIC          "aesd/test"

char mqtt_test_message[] = "Hello from MQTT Publisher!";

int main(int argc, char* argv[])
{
    openlog(NULL, 0, LOG_USER);
    int result;

    struct mosquitto* mqtt_client;
    mosquitto_lib_init();
    mqtt_client = mosquitto_new(NULL, true, NULL);
    if (!mqtt_client)
    {
        syslog(LOG_ERR, "Failed to create Mosquitto instance");
        goto cleanup;
    }

    // Connect to MQTT broker
    if ((result = mosquitto_connect(mqtt_client, MQTT_BROKER_ADDR, MQTT_BROKER_PORT, 60)) != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to connect to MQTT broker: %s", mosquitto_strerror(result));
        goto cleanup;
    }

    // Publish test message
    if ((result = mosquitto_publish(mqtt_client, NULL, MQTT_TOPIC, sizeof(mqtt_test_message), mqtt_test_message, 0, false)) != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to publish message to topic %s", mosquitto_strerror(result));
        goto cleanup;
    }

cleanup:
    mosquitto_destroy(mqtt_client);
    mosquitto_lib_cleanup();
    return 0;
}