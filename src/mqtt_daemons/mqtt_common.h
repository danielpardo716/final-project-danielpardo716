#ifndef MQTT_COMMON_H
#define MQTT_COMMON_H

#include "mosquitto.h"
#include "mqtt_defs.h"

void mqtt_init(struct mosquitto** mqtt_client, char* username, void (*cleanup_and_exit)(int), void (*msg_callback)(struct mosquitto*, void*, const struct mosquitto_message*));
void mqtt_wait_for_connection(struct mosquitto* mqtt_client, void (*cleanup_and_exit)(int));
void mqtt_close(struct mosquitto** mqtt_client);

#endif