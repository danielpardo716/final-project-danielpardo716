/* THIS FILE IS NOT TO BE INCLUDED
To use, copy to mqtt_defs.h and replace MQTT_BROKER_ADDR
with the actual IP address.
*/

#ifndef MQTT_DEFS_H
#define MQTT_DEFS_H

#define MQTT_BROKER_ADDR        "localhost"                     ///< IP of broker server
#define MQTT_BROKER_PORT        1883                            ///< Default listener port
#define MQTT_PASSWORD           "password1234"                  ///< Example password
#define MQTT_MAX_TIMEOUT_S      120                             ///< Connection/TLS handshake timeout, in seconds
#define MQTT_CAFILE             "/etc/mosquitto/ca.crt"         ///< TLS Certificate Authority (CA) certificate file
#define MQTT_CERTFILE           "/etc/mosquitto/client.crt"     ///< TLS Client certificate file
#define MQTT_KEYFILE            "/etc/mosquitto/client.key"     ///< TLS Client private key file
#define MQTT_TOPIC              "aesd/sensors/ambient"          ///< MQTT topic to publish/subscribe to

#endif