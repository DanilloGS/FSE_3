#ifndef FLASH_MEMO_H
#define FLASH_MEMO_H

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

int32_t le_valor_nvs();
void grava_valor_nvs(int32_t valor);

#endif