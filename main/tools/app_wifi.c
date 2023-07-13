//
// Created by songdehuai on 2023/1/16.
//


#include <esp_mac.h>
#include "app_wifi.h"


app_st_wifi app_g_wifi = {
        .mqtt_connect = false,
        .wifi_connect = false,
};

static const char *TAG = "Application WIFI";

static EventGroupHandle_t s_wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const int ESPTOUCH_STOP_BIT = BIT2;

app_event_wifi_success *event_wifi_success;

app_event_wifi_success event_wifi_error;


void smart_config_task(void *parm) {
    ESP_LOGI(TAG, "启动配网");
    app_g_wifi.wifi_connect = false;
    esp_wifi_disconnect();
    esp_wifi_scan_stop();
    esp_smartconfig_stop();
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_V2));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT | ESPTOUCH_STOP_BIT, true, false, portMAX_DELAY);
        if (uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "WIFI已经连接");
            vTaskDelete(NULL);
        }
        if (uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "配网完毕");
            esp_wifi_scan_stop();
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
        if (uxBits & ESPTOUCH_STOP_BIT) {
            ESP_LOGI(TAG, "配网停止");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}

static void smart_config_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WIFI启动");
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT | ESPTOUCH_STOP_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        //连接成功
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {

        //获取WIFI名
        wifi_config_t wifi_config = {0};
        esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
        bzero(app_g_wifi.wifi, sizeof(app_g_wifi.wifi));
        sprintf(app_g_wifi.wifi, "%s", wifi_config.sta.ssid);


        //获取IP
        esp_netif_ip_info_t local_ip;
        esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &local_ip);
        bzero(app_g_wifi.ip_local, sizeof(app_g_wifi.ip_local));
        sprintf(app_g_wifi.ip_local, IPSTR, IP2STR(&local_ip.ip));

        ESP_LOGI(TAG, "获得IP: %s", app_g_wifi.ip_local);

        app_g_wifi.wifi_connect = true;

        if (event_wifi_success) { event_wifi_success(); }

        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);

    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "扫描完成");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(TAG, "发现通道");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG, "获取SSID和密码");
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *) event_data;
        wifi_config_t wifi_config;

        uint8_t ssid[32] = {0};
        uint8_t password[64] = {0};
        uint8_t rvd_data[33] = {0};

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            ESP_LOGI(TAG, "bssid:%d", 1);
            //todo 解出主控IP地址
            for (int i = 0; i < sizeof(wifi_config.sta.bssid); i++) {
                printf("%02x ", evt->bssid[i]);
            }
            printf("\n");
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));

        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK(esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
            ESP_LOGI(TAG, "RVD_DATA:");
            for (int i = 0; i < 33; i++) {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        app_flash_write(ARGS_WIFI_NAME, ssid, sizeof(ssid));
        app_flash_write(ARGS_WIFI_PASS, password, sizeof(password));

        ESP_LOGI(TAG, "保存WIFI信息");

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

void app_wifi_init() {
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_net_if = esp_netif_create_default_wifi_sta();
    assert(sta_net_if);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &smart_config_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &smart_config_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &smart_config_event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_config_t wifi_config;
    if ((app_flash_read(ARGS_WIFI_NAME, app_g_wifi.wifi, sizeof(app_g_wifi.wifi))) && (app_flash_read(ARGS_WIFI_PASS, app_g_wifi.wifi_passwd, sizeof(app_g_wifi.wifi_passwd)))) {
        ESP_LOGI(TAG, "连接保存的WIFI");
        //使用获取的配网信息链接无线网络
        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, app_g_wifi.wifi, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, app_g_wifi.wifi_passwd, sizeof(wifi_config.sta.password));

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else {
        ESP_LOGI(TAG, "没有发现保存的WIFI,启动配网");
        app_wifi_smart_config_start();
    }

}

void app_wifi_event_register(app_event_wifi_success *event) {
    event_wifi_success = *event;
}

void app_wifi_event_unregister() {
    event_wifi_success = NULL;
}

void app_wifi_smart_config_start() {
    xTaskCreate(smart_config_task, "smart_config_task", 1024 * 4, NULL, 8, NULL);
}

void app_wifi_smart_config_stop() {
    xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_STOP_BIT);
}

