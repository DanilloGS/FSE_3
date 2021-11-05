#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "dht11.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "mqtt.h"
// #include "nvs_flash.h"
#include "flash_memo.h"
#include "wifi.h"

#define LED 2
#define BOTAO 0

char esp_topic[50], mac_address[13], temp_topic[50], hum_topic[50], state_topic[50];
int esp_state, first_connection;

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;
xQueueHandle filaDeInterrupcao;

void init_button() {
  gpio_pad_select_gpio(LED);
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);

  gpio_pad_select_gpio(BOTAO);
  gpio_set_direction(BOTAO, GPIO_MODE_INPUT);

  esp_state = le_valor_nvs("esp_state", "", integer_type);
  gpio_set_level(LED, esp_state);

  gpio_pulldown_en(BOTAO);
  gpio_pullup_dis(BOTAO);

  gpio_set_intr_type(BOTAO, GPIO_INTR_NEGEDGE);
}

void set_mac() {
  uint8_t mac_base[6] = {0};
  esp_efuse_mac_get_default(mac_base);
  sprintf(mac_address, "%02X%02X%02X%02X%02X%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
}

void set_esp_topic() {
  set_mac();
  strcpy(esp_topic, "fse2021/170139981/dispositivos/");
  strcat(esp_topic, mac_address);
}

void set_house_topics() {
  char house_topic[50], aux_house_topic[50];
  le_valor_nvs("house_topic", house_topic, string_type);
  strcpy(aux_house_topic, hum_topic);

  strcat(house_topic, "/temperatura");
  strcpy(temp_topic, house_topic);

  strcpy(hum_topic, aux_house_topic);
  strcat(hum_topic, "/umidade");

  strcpy(hum_topic, aux_house_topic);
  strcat(state_topic, "/estado");
}

void connect_wifi(void *params) {
  while (true) {
    if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)) {
      mqtt_start(esp_topic);
    }
  }
}

void handle_server(void *params) {
  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
    // Atrasar a ligação do sensor
    set_house_topics();
#ifdef CONFIG_ENERGIA
    while (true) {
      if (esp_state == 1) {
        struct dht11_reading temp_hum = avarage_value(5);
        printf("temp %d hum: %d status: %d\n", temp_hum.temperature, temp_hum.humidity, temp_hum.status);

        cJSON *json_response_temp = cJSON_CreateObject();
        cJSON *json_response_humid = cJSON_CreateObject();

        cJSON_AddNumberToObject(json_response_temp, "temperature", temp_hum.temperature);
        cJSON_AddStringToObject(json_response_temp, "mac", mac_address);
        mqtt_envia_mensagem(temp_topic, cJSON_Print(json_response_temp));

        cJSON_AddNumberToObject(json_response_humid, "humidity", temp_hum.humidity);
        cJSON_AddStringToObject(json_response_humid, "mac", mac_address);
        mqtt_envia_mensagem(hum_topic, cJSON_Print(json_response_humid));

        cJSON_Delete(json_response_temp);
        cJSON_Delete(json_response_humid);
      }
    }
#endif
#ifdef CONFIG_BATERIA
// ToDo
#endif
  }
}

static void IRAM_ATTR gpio_isr_handler(void *args) {
  int pino = (int)args;
  xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void handle_gipo(void *params) {
  int pino;

  while (true) {
    if (xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY)) {
      // De-bouncing
      int estado = gpio_get_level(pino);
      if (estado == 0) {
        printf("Apertou o botão\n");

        if (esp_state == 0) {
          esp_state = 1;
        } else {
          esp_state = 0;
        }

        gpio_set_level(LED, esp_state);
        vTaskDelay(50 / portTICK_PERIOD_MS);

        cJSON *json_response = cJSON_CreateObject();
        cJSON_AddNumberToObject(json_response, "status", esp_state);
        cJSON_AddStringToObject(json_response, "mac", mac_address);

        mqtt_envia_mensagem(esp_topic, cJSON_Print(json_response));

        grava_value_nvs("esp_state", esp_state, "", integer_type);
        printf("Estado da ESP conectada: %d\n", esp_state);

        gpio_isr_handler_remove(pino);

        while (gpio_get_level(pino) == estado) {
          vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_isr_handler_add(pino, gpio_isr_handler, (void *)pino);
      }
    }
  }
}

void app_main(void) {
  int result = le_valor_nvs("esp_topic", esp_topic, string_type);
  if (result == -1) {
    first_connection = 1;
    set_esp_topic();
    grava_value_nvs("esp_topic", 0, esp_topic, string_type);
    grava_value_nvs("esp_state", 1, "", integer_type);
  } else {
    first_connection = 0;
    set_mac();
    printf("Topico da ESP atual: %s\n", esp_topic);
  }

  init_button();

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  conexaoWifiSemaphore = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore = xSemaphoreCreateBinary();
  wifi_start();

  filaDeInterrupcao = xQueueCreate(10, sizeof(int));
  xTaskCreate(&handle_gipo, "TrataBotao", 2048, NULL, 1, NULL);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BOTAO, gpio_isr_handler, (void *)BOTAO);

  xTaskCreate(&connect_wifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&handle_server, "Comunicação com Broker", 4096, NULL, 1, NULL);
  xSemaphoreGive(conexaoMQTTSemaphore);
}
