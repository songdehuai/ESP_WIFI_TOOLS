//
// Created by songdehuai on 2023/1/16.
//

#ifndef ESP32C3_RGB_APP_WIFI_H
#define ESP32C3_RGB_APP_WIFI_H

#include "include/app_include.h"

typedef void (app_event_wifi_success)(void);

typedef struct app_st_wifi {
    //wifi
    char wifi[32];
    //wifi 密码
    char wifi_passwd[64];
    //本地ip
    char ip_local[16];
    //设备名
    uint8_t host_name[6];
    //WIFI是否连接
    bool wifi_connect;
    //MQTT是否连接
    bool mqtt_connect;

} app_st_wifi;

extern app_st_wifi app_g_wifi;

void app_wifi_init();

void app_wifi_event_register(app_event_wifi_success *event);

void app_wifi_event_unregister();

void app_wifi_smart_config_start();

void app_wifi_smart_config_stop();

#endif //ESP32C3_RGB_APP_WIFI_H