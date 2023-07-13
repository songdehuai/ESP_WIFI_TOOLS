//
// Created by songdehuai on 2023/2/17.
//

#ifndef ESP_WIFI_TOOLS_APP_INCLUDE_H
#define ESP_WIFI_TOOLS_APP_INCLUDE_H


#include "nvs.h"
#include "nvs_flash.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>


#include <esp_log.h>
#include <esp_types.h>
#include <esp_wifi.h>
#include <esp_smartconfig.h>
#include <esp_http_server.h>
#include <esp_system.h>
#include <esp_event.h>
#include <cJSON.h>

#include <mqtt_client.h>

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"


#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>

#include "iot_button.h"

#include "tools/app_wifi.h"
#include "tools/app_flash.h"
#include "tools/app_mqtt.h"

#endif //ESP_WIFI_TOOLS_APP_INCLUDE_H
