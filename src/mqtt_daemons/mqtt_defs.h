#ifndef MQTT_DEFS_H
#define MQTT_DEFS_H

#define MQTT_BROKER_ADDR        "192.168.1.169"     // IP of broker server
#define MQTT_BROKER_PORT        8883                // Default listener port
#define MQTT_MAX_TIMEOUT_S      120
#define MQTT_CAFILE             "/etc/mosquitto/certs/ca.crt"
#define MQTT_CERTFILE           "/etc/mosquitto/certs/client.crt"
#define MQTT_KEYFILE            "/etc/mosquitto/certs/client.key"
#define MQTT_TOPIC              "aesd/sensors/ambient"

#endif