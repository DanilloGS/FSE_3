#include <stdio.h>
#include <string.h>

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

// #define LOW_POWER CONFIG_ESP_LOW_POWER

// xSemaphoreHandle conexaoWifiSemaphore;
// xSemaphoreHandle conexaoMQTTSemaphore;

// void conectadoWifi(void* params) {
//   while (true) {
//     if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)) {
//       // Processamento Internet
//       mqtt_start();
//     }
//   }
// }

// void trataComunicacaoComServidor(void* params) {
//   char mensagem[50];
//   if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
//     while (true) {
//       float temperatura = 20.0 + (float)rand() / (float)(RAND_MAX / 10.0);
//       sprintf(mensagem, "temperatura1: %f", temperatura);
//       mqtt_envia_mensagem("sensores/temperatura", mensagem);
//       vTaskDelay(3000 / portTICK_PERIOD_MS);
//     }
//   }
// }

// #ifdef CONFIG_ENERGIA
// void print(){
//   printf("CONFIG_ENERGIA\n");
// }
// #endif

// #ifdef CONFIG_BATERIA
// void print(){
//   printf("CONFIG_BATERIA\n");
// }
// #endif

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

void app_main(void) {
  // print();
  get_mac_addr();
}
