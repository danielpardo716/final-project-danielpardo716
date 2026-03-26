/* THIS FILE IS NOT TO BE INCLUDED
To use, copy to mqtt_defs.h and replace MQTT_BROKER_ADDR
with the actual IP address.
*/

#ifndef MQTT_DEFS_H
#define MQTT_DEFS_H

#define MQTT_BROKER_ADDR        "localhost"         // IP of broker server
#define MQTT_BROKER_PORT        1883                // Default listener port
#define MQTT_MAX_TIMEOUT_S      120
#define MQTT_CAFILE             "/etc/mosquitto/ca.crt"
#define MQTT_CERTFILE           "/etc/mosquitto/server.crt"
#define MQTT_KEYFILE            "/etc/mosquitto/server.key"
#define MQTT_TOPIC              "aesd/sensors/ambient"

#endif