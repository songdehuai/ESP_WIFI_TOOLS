/*
 * @Author: slx
 * @Date: 2022-03-02 22:47:31
 * @LastEditors: slx
 * @LastEditTime: 2022-03-03 15:35:56
 * @Description: 存储flash
 */
#include "app_flash.h"

bool is_flash_init = false;

/**
 * 初始化nvs_flash
 * @return
 */
void app_flash_init() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    is_flash_init = true;
}

/**
 * @description: 保存数据
 * @param {char} *key 关键字
 * @param {void} *outbuf返回数据
 * @param {uint32_t} len读取到的长度
 * @return {*} 读取成功返回true 失败false
 */
bool app_flash_read(const char *key, void *outbuf, uint32_t len) {
    if (!is_flash_init) {
        ESP_LOGE("flash", "nvs_flash not initialized");
        return false;
    }
    bool res = false;
    esp_err_t err = ESP_OK;
    uint32_t r_len = len;
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open(FLASH_TABLE, NVS_READWRITE, &nvs_handle));
    err |= nvs_get_blob(nvs_handle, key, outbuf, &r_len); //读数据
    if (err == ESP_OK) {
        res = true;
    }
    nvs_close(nvs_handle);
    return res;
}


/**
 * @description: 写flash数据
 * @param {char} *key关键字
 * @param {void} *wbuf要写入的数据
 * @param {uint32_t}len长度
 * @return {*}
 */
bool app_flash_write(const char *key, void *wbuf, uint32_t len) {
    if (!is_flash_init) {
        ESP_LOGE("flash", "nvs_flash not initialized");
        return false;
    }
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open(FLASH_TABLE, NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_blob(nvs_handle, key, wbuf, len));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
    return true;
}

