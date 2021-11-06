#include "flash_memo.h"

int32_t le_valor_nvs(char *value_name, char *str_value, int value_type) {
  // Inicia o acesso à partição padrão nvs
  // ESP_ERROR_CHECK(nvs_flash_init());

  // Inicia o acesso à partição personalizada
  ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));

  int32_t valor = 0;
  nvs_handle particao_padrao_handle;

  // Abre o acesso à partição nvs
  // esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &particao_padrao_handle);

  // Abre o acesso à partição DadosNVS
  esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READONLY, &particao_padrao_handle);

  if (res_nvs == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
    return -1;
  } else {
    esp_err_t res;
    char *real_string;
    if (value_type == 0) {
      res = nvs_get_i32(particao_padrao_handle, value_name, &valor);
    } else {
      size_t str_size;
      res = nvs_get_str(particao_padrao_handle, value_name, NULL, &str_size);
      real_string = malloc(sizeof(char) * str_size);
      res = nvs_get_str(particao_padrao_handle, value_name, real_string, &str_size);
    }

    switch (res) {
      case ESP_OK:
        if (value_type == 0)
          printf("Valor armazenado: %d\n", valor);
        else {
          strcpy(str_value, real_string);
          printf("Valor armazenado: %s\n", real_string);
          // free(real_string);
        }
        break;
      case ESP_ERR_NOT_FOUND:
        // if (value_type != 0) free(real_string);
        ESP_LOGE("NVS", "Valor não encontrado");
        return -1;
      default:
        // if (value_type != 0) free(real_string);
        ESP_LOGE("NVS", "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
        return -1;
        break;
    }
    nvs_close(particao_padrao_handle);
  }
  return valor;
}

void grava_value_nvs(char *value_name, int32_t valor, char *str_value, int value_type) {
  ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));
  nvs_handle particao_padrao_handle;

  esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READWRITE, &particao_padrao_handle);

  if (res_nvs == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
  }

  esp_err_t res;
  if (value_type == 0) {
    res = nvs_set_i32(particao_padrao_handle, value_name, valor);
    printf("Lendo int\n");
  } else {
    res = nvs_set_str(particao_padrao_handle, value_name, str_value);
    printf("Lendo string\n");
  }
  if (res != ESP_OK) {
    ESP_LOGE("NVS", "Não foi possível escrever no NVS (%s)", esp_err_to_name(res));
  }
  nvs_commit(particao_padrao_handle);
  nvs_close(particao_padrao_handle);
}
