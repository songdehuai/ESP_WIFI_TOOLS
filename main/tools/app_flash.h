#ifndef _U_FLASH_H
#define _U_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "include/app_include.h"


#define FLASH_TABLE             "FLASH_TABLE"             //表
#define ARGS_WIFI_NAME          "ARGS_WIFI_NAME"          //wifi名称
#define ARGS_WIFI_PASS          "ARGS_WIFI_PASS"          //wifi密码
#define ARGS_RF_433_ID          "ARGS_RF_433_ID"          //433通信id

void app_flash_init();

bool app_flash_read(const char *key, void *outbuf, uint32_t len);

bool app_flash_write(const char *key, void *wbuf, uint32_t len);

#ifdef  __cplusplus
}
#endif

#endif
