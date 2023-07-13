//
// Created by songdehuai on 2023/2/17.
//

#include "app_mqtt.h"

static const char *TAG = "Application MQTT";

esp_mqtt_client_handle_t mqtt_client;

app_event_mqtt *app_event_mqtt_connect;


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t) event_id) {
        case MQTT_EVENT_CONNECTED:
            app_g_wifi.mqtt_connect = true;
            if (app_event_mqtt_connect) {
                app_event_mqtt_connect(app_g_wifi.mqtt_connect);
            }
            esp_mqtt_client_subscribe(mqtt_client, "/topic/down", 2);


            uint32_t randomValue = esp_random();
            char randomString[10];
            sprintf(randomString, "%lu", randomValue);
            esp_mqtt_client_publish(mqtt_client, "/topic/up", randomString, 0, 2, 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            app_g_wifi.mqtt_connect = false;
            if (app_event_mqtt_connect) {
                app_event_mqtt_connect(app_g_wifi.mqtt_connect);
            }
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            ESP_LOGI(TAG, "TOPIC=%.*s\r\n", event->topic_len, event->topic);
            ESP_LOGI(TAG, "DATA=%.*s\r\n", event->data_len, event->data);
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void app_mqtt_init() {
    char mqttUrl[200] = "mqtt://";
    strcat(mqttUrl, "10.10.10.197");
    strcat(mqttUrl, ":1883");
    esp_mqtt_client_config_t mqtt_cfg = {
            .broker = {
                    .address = {
                            .uri = mqttUrl,
                    },
            },
//            .session = {
//                    .disable_clean_session = false,
//                    .keepalive = 30,
//                    .disable_keepalive = false,
//            },
//            .network = {
//                    .timeout_ms = 30,
//            },
            .credentials = {
                    .client_id = "mqtt_test_231",
                    .username = "mqtt_test",
                    .authentication = {
                            .password = "mqtt_test"
                    }
            },
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
}

void app_mqtt_start() {
    esp_mqtt_client_start(mqtt_client);
}

void app_mqtt_stop() {
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_stop(mqtt_client);
    app_g_wifi.mqtt_connect = false;
    if (app_event_mqtt_connect) {
        app_event_mqtt_connect(app_g_wifi.mqtt_connect);
    }
}

void app_mqtt_event_register(app_event_mqtt *event) {
    app_event_mqtt_connect = *event;
}

void app_mqtt_event_unregister() {
    app_event_mqtt_connect = NULL;
}