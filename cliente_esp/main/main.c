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

#define LED_ESP 2
#define BOTAO 15
#define SENSOR 15

char topic[50], mac_address[13];

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;

void handle_touch() {
  // Configuração dos pinos dos LEDs
  gpio_pad_select_gpio(LED_ESP);
  gpio_set_direction(LED_ESP, GPIO_MODE_OUTPUT);

  // Configuração do pino do Botão
  gpio_pad_select_gpio(BOTAO);
  gpio_set_direction(BOTAO, GPIO_MODE_INPUT);
  gpio_pulldown_en(BOTAO);
  gpio_pullup_dis(BOTAO);

  int estado_botao = gpio_get_level(BOTAO);
  gpio_set_level(LED_ESP, estado_botao);
}

void set_mac() {
  uint8_t mac_base[6] = {0};
  esp_efuse_mac_get_default(mac_base);
  sprintf(mac_address, "%02X%02X%02X%02X%02X%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
}

void set_topic() {
  set_mac();
  strcpy(topic, "fse2021/170139981/dispositivos/");
  strcat(topic, mac_address);
}

struct dht11_reading avarage_value(int count) {
  struct dht11_reading avarage = {0, 0, 0};
  for (size_t i = 0; i < count; i++) {
    avarage.temperature += DHT11_read().temperature;
    avarage.humidity += DHT11_read().humidity;
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
  avarage.temperature = avarage.temperature / count;
  avarage.humidity = avarage.humidity / count;
  return avarage;
}

// void conectadoWifi(void* params) {
//   while (true) {
//     if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)) {
//       // Processamento Internet
//       mqtt_start(topic);
//     }
//   }
// }

// void trataComunicacaoComServidor(void* params) {
//   if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
//     // #ifdef CONFIG_ENERGIA
//     while (true) {
//       struct dht11_reading temp_hum = avarage_value();
//       cJSON* json_response = cJSON_CreateObject();
//       cJSON_AddNumberToObject(json_response, "temperature", temp_hum.temperature);
//       mqtt_envia_mensagem("1", cJSON_Print(resHumidity));
//       cJSON_AddNumberToObject(json_response, "humidity", temp_hum.humidity);
//       //   mqtt_envia_mensagem("")
//       //   // ToDo
//       // }
// // #endif
// #ifdef CONFIG_BATERIA
// // ToDo
// #endif
//     }
//   }

void app_main(void) {
  DHT11_init();

  int result = le_valor_nvs("topic", topic, 1);
  
  if (result == -1) {
    set_topic();
    grava_value_nvs("topic", 0, topic, 1);
  } else {
    printf("Topico da ESP atual: %s\n", topic);
  }

  // conexaoWifiSemaphore = xSemaphoreCreateBinary();
  // conexaoMQTTSemaphore = xSemaphoreCreateBinary();
  // wifi_start();

  // xTaskCreate(&conectadoWifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
  // xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
  // handle_touch();
}
