#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "mqtt_common.h"

static bool mqtt_connected = false;

static int file_exists(const char* path)
{
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

static void mqtt_connect_callback(struct mosquitto* mosq, void* obj, int rc)
{
    if (rc != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to connect to MQTT broker: %s", mosquitto_strerror(rc));
    }
    else
    {
        mqtt_connected = true;
    }
}

static void mqtt_log_callback(struct mosquitto* mosq, void* obj, int level, const char* str)
{
    syslog(LOG_INFO, "MQTT log [%d]: %s", level, str);
}

void mqtt_init(struct mosquitto** mqtt_client, char* username, void (*cleanup_and_exit)(int), void (*msg_callback)(struct mosquitto*, void*, const struct mosquitto_message*))
{
    int result; 

    mosquitto_lib_init();
    *mqtt_client = mosquitto_new(NULL, true, NULL);
    if (!(*mqtt_client))
    {
        syslog(LOG_ERR, "Failed to create Mosquitto instance: %s", strerror(errno));
        cleanup_and_exit(EXIT_FAILURE);
    }

    // Configure username/password
    if ((result = mosquitto_username_pw_set(*mqtt_client, username, MQTT_PASSWORD)) != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to set username/password: %s", mosquitto_strerror(result));
        cleanup_and_exit(result);
    }
    
    // Register callbacks
    mosquitto_connect_callback_set(*mqtt_client, mqtt_connect_callback);
    mosquitto_log_callback_set(*mqtt_client, mqtt_log_callback);
    if (msg_callback != NULL)
    {
        mosquitto_message_callback_set(*mqtt_client, msg_callback);
    }
    
    // Verify certificate files exist
    if (!file_exists(MQTT_CAFILE))
    {
        syslog(LOG_ERR, "CA certificate not found: %s", MQTT_CAFILE);
        cleanup_and_exit(EXIT_FAILURE);
    }
    if (!file_exists(MQTT_CERTFILE))
    {
        syslog(LOG_ERR, "Client certificate not found: %s", MQTT_CERTFILE);
        cleanup_and_exit(EXIT_FAILURE);
    }
    if (!file_exists(MQTT_KEYFILE))
    {
        syslog(LOG_ERR, "Client key not found: %s", MQTT_KEYFILE);
        cleanup_and_exit(EXIT_FAILURE);
    }
    
    // Set up TLS encryption
    if ((result = mosquitto_tls_set(*mqtt_client, MQTT_CAFILE, NULL, MQTT_CERTFILE, MQTT_KEYFILE, NULL)) != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to enable TLS encryption: %s", mosquitto_strerror(result));
        cleanup_and_exit(result);
    }

    // Connect to MQTT broker
    if ((result = mosquitto_connect(*mqtt_client, MQTT_BROKER_ADDR, MQTT_BROKER_PORT, 60)) != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to connect to MQTT broker: %s", mosquitto_strerror(result));
        cleanup_and_exit(result);
    }
}

void mqtt_wait_for_connection(struct mosquitto* mqtt_client, void (*cleanup_and_exit)(int))
{
    int result;

    // Start mosquitto loop in a new thread
    if ((result = mosquitto_loop_start(mqtt_client)) != MOSQ_ERR_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to start loop: %s", mosquitto_strerror(result));
        cleanup_and_exit(result);
    }

    // Wait for connection and TLS handshake to complete
    int timeout_count = 0;
    syslog(LOG_INFO, "Waiting for MQTT connection to %s:%d...", MQTT_BROKER_ADDR, MQTT_BROKER_PORT);
    while (!mqtt_connected && (timeout_count < (MQTT_MAX_TIMEOUT_S)))
    {
        sleep(1);
        ++timeout_count;
    }

    if (!mqtt_connected)
    {
        syslog(LOG_ERR, "Timed out waiting for TLS handshake after %d seconds", timeout_count);        
        cleanup_and_exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "MQTT connection established, starting sensor readings");
}

void mqtt_close(struct mosquitto** mqtt_client)
{
    if (*mqtt_client != NULL)
    {
        mosquitto_loop_stop(*mqtt_client, true);
        mosquitto_destroy(*mqtt_client);
    }
    mosquitto_lib_cleanup();
    *mqtt_client = NULL;
}