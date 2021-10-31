#include <stdio.h>
#include <string.h>

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
#include "nvs_flash.h"
#include "wifi.h"

#define LED_ESP 2
#define BOTAO 15
#define SENSOR 15

// #define LOW_POWER CONFIG_ESP_LOW_POWER

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

char* get_mac_addr() {
  uint8_t mac_base[6] = {0};
  char mac_addr[13];
  esp_efuse_mac_get_default(mac_base);
  sprintf(mac_addr, "%02X%02X%02X%02X%02X%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
  return mac_addr;
}

struct dht11_reading avarage_value() {
  struct dht11_reading avarage = {0, 0, 0};
  for (size_t i = 0; i < 5; i++) {
    avarage.temperature += 20.0 + (float)rand() / (float)(RAND_MAX / 10.0);
    // avarage.temperature += DHT11_read().temperature;
    // avarage.humidity += DHT11_read().humidity;
    avarage.humidity += 60.0 + (float)rand() / (float)(RAND_MAX / 10.0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
  avarage.temperature = avarage.temperature / 5;
  avarage.humidity = avarage.humidity / 5;
  return avarage;
}

void conectadoWifi(void* params) {
  while (true) {
    if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)) {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void trataComunicacaoComServidor(void* params) {
  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
    while (true) {
#ifdef CONFIG_ENERGIA
      struct dht11_reading temp_hum = avarage_value();
#endif
#ifdef CONFIG_BATERIA
// ToDo
#endif
    }
  }
}


void app_main(void) {
  DHT11_init();
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  conexaoWifiSemaphore = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore = xSemaphoreCreateBinary();
  wifi_start();

  xTaskCreate(&conectadoWifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
