#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "dht11.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_wifi.h"
#include "flash_memo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "mqtt.h"
#include "wifi.h"

#define LED 2
#define BOTAO 0
#define BUTTON_PIN_BITMASK 0x200000000

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
  char house_topic[50], aux_house_topic[50] = "fse2021/170139981/";
  le_valor_nvs("house_topic", house_topic, string_type);
  strcat(aux_house_topic, house_topic);

#ifdef CONFIG_ENERGIA
  strcpy(house_topic, aux_house_topic);
  strcat(house_topic, "/temperatura");
  strcpy(temp_topic, house_topic);

  strcpy(house_topic, aux_house_topic);
  strcat(house_topic, "/umidade");
  strcpy(hum_topic, house_topic);
#endif

  strcpy(house_topic, aux_house_topic);
  strcat(house_topic, "/estado");
  strcpy(state_topic, house_topic);
}

static void IRAM_ATTR gpio_isr_handler(void *args) {
  int pino = (int)args;
  xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void handle_gpio(void *params) {
  int pino;
  while (true) {
    if (xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY)) {
      // De-bouncing
      int estado = gpio_get_level(pino);
      if (estado == 0) {
        gpio_set_level(LED, 1);
        printf("Apertou o botão\n");

        esp_state = !le_valor_nvs("esp_state", "", integer_type);
        vTaskDelay(50 / portTICK_PERIOD_MS);

        grava_value_nvs("esp_state", esp_state, "", integer_type);

        gpio_isr_handler_remove(pino);

        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_isr_handler_add(pino, gpio_isr_handler, (void *)pino);
      }
      gpio_set_level(LED, 0);
    }
  }
}

void connect_wifi(void *params) {
  while (true) {
    if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)) {
      mqtt_start(esp_topic, mac_address);
    }
  }
}

void handle_server(void *params) {
  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
    set_house_topics();
#ifdef CONFIG_ENERGIA
    while (true) {
      cJSON *json_response_default = cJSON_CreateObject();
      esp_state = le_valor_nvs("esp_state", "", integer_type);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      if (esp_state) {
        struct dht11_reading temp_hum = avarage_value(5);
        cJSON_AddNumberToObject(json_response_default, "temperature", temp_hum.temperature);
        cJSON_AddNumberToObject(json_response_default, "humidity", temp_hum.humidity);
      } else {
        vTaskDelay(9000 / portTICK_PERIOD_MS);
      }

      cJSON_AddStringToObject(json_response_default, "type", "energia");
      cJSON_AddNumberToObject(json_response_default, "json_type", 2);
      cJSON_AddNumberToObject(json_response_default, "state", esp_state);
      cJSON_AddStringToObject(json_response_default, "mac", mac_address);

      mqtt_envia_mensagem(esp_topic, cJSON_Print(json_response_default));
    }
#endif
#ifdef CONFIG_BATERIA
    printf("Atualizando estado da ESP\n");

    rtc_gpio_pullup_en(BOTAO);
    rtc_gpio_pulldown_dis(BOTAO);
    esp_sleep_enable_ext0_wakeup(BOTAO, 0);
    gpio_set_level(LED, 1);
    esp_state = le_valor_nvs("esp_state", "", integer_type);
    esp_state = !esp_state;
    vTaskDelay(500 / portTICK_PERIOD_MS);
    cJSON *json_response_status = cJSON_CreateObject();

    cJSON_AddNumberToObject(json_response_status, "status", esp_state);
    cJSON_AddStringToObject(json_response_status, "mac", mac_address);
    cJSON_AddStringToObject(json_response_status, "type", "bateria");
    cJSON_AddNumberToObject(json_response_status, "json_type", 2);

    mqtt_envia_mensagem(esp_topic, cJSON_Print(json_response_status));

    grava_value_nvs("esp_state", esp_state, "", integer_type);
    gpio_set_level(LED, 0);
    printf("Ativando economia de energia\n");
    esp_deep_sleep_start();

#endif
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
  xTaskCreate(handle_gpio, "TrataBotao", 2048, NULL, 1, NULL);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BOTAO, gpio_isr_handler, (void *)BOTAO);

  xTaskCreate(&connect_wifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&handle_server, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
