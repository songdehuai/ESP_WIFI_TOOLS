#include "include/app_include.h"

static char *TAG = "Application Main";


void button_long_click() {
    app_wifi_smart_config_start();
}

void button_double_click() {
    app_mqtt_start();
}

void button_click() {
    app_mqtt_stop();
}

void app_wifi_success() {
    ESP_LOGI(TAG, "WI-FI连接成功");
    app_mqtt_start();
}

void app_mqtt_connect(bool connect) {
    if (connect) {
        ESP_LOGI(TAG, "MQTT连接");
    } else {
        ESP_LOGI(TAG, "MQTT断开");
    }
}


void app_main(void) {
    ESP_LOGI(TAG, "run");

    app_flash_init();

    app_wifi_init();

    app_wifi_event_register(app_wifi_success);

    app_mqtt_init();

    app_mqtt_event_register(app_mqtt_connect);


    button_handle_t gpio_btn;
    button_config_t gpio_btn_cfg = {
            .type = BUTTON_TYPE_GPIO,
            .long_press_time = 1500,
            .short_press_time = 500,
            .gpio_button_config = {
                    .gpio_num = 9,
                    .active_level = 0,
            },
    };
    gpio_btn = iot_button_create(&gpio_btn_cfg);
    if (NULL == gpio_btn) {
        ESP_LOGE(TAG, "Button create failed");
    }
    iot_button_register_cb(gpio_btn, BUTTON_LONG_PRESS_START, button_long_click, NULL);
    iot_button_register_cb(gpio_btn, BUTTON_DOUBLE_CLICK, button_double_click, NULL);
    iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, button_click, NULL);

}

