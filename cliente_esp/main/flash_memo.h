#ifndef FLASH_MEMO_H
#define FLASH_MEMO_H
#define integer_type 0
#define string_type 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"


int32_t le_valor_nvs(char *value_name, char *str_value, int value_type);
void grava_value_nvs(char *value_name, int32_t valor, char *str_value, int value_type);

#endif