//
// Created by songdehuai on 2023/2/17.
//

#ifndef ESP_WIFI_TOOLS_APP_MQTT_H
#define ESP_WIFI_TOOLS_APP_MQTT_H

#include "include/app_include.h"

typedef void (app_event_mqtt)(bool);

void app_mqtt_init();

void app_mqtt_start();

void app_mqtt_stop();

void app_mqtt_event_register(app_event_mqtt *event);

void app_mqtt_event_unregister();

#endif //ESP_WIFI_TOOLS_APP_MQTT_H
