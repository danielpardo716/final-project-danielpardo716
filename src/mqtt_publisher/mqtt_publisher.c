#include <syslog.h>
#include "mosquitto.h"

#define MQTT_BROKER_ADDR    "localhost"
#define MQTT_BROKER_PORT    1883
#define MQTT_TOPIC          "aesd/sensors/ambient"

char mqtt_test_message[] = "Hello from MQTT Publisher!";

int main(int argc, char* argv[])
{
    openlog(NULL, 0, LOG_USER);
    int result;

    struct mosquitto* mosq;
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq)
    {
        syslog(LOG_ERR, "Failed to create Mosquitto instance");
        goto cleanup;
    }

    // Connect to MQTT broker
    if ((result = mosquitto_connect(mosq, MQTT_BROKER_ADDR, MQTT_BROKER_PORT, 60)) != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to connect to MQTT broker: %s", mosquitto_strerror(result));
        goto cleanup;
    }

    // Publish test message
    if ((result = mosquitto_publish(mosq, NULL, MQTT_TOPIC, sizeof(mqtt_test_message), mqtt_test_message, 0, false)) != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to publish message to topic %s", mosquitto_strerror(result));
        goto cleanup;
    }

cleanup:
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}