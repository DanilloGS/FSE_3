#include "mqtt.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "flash_memo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "mqtt_client.h"

#define TAG "MQTT"

extern int first_connection;
char mac_address[50];
char topic_name[50];

extern xSemaphoreHandle conexaoMQTTSemaphore;
esp_mqtt_client_handle_t client;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event) {
  esp_mqtt_client_handle_t client = event->client;

  switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
      if (first_connection) {
        cJSON *json_connect_esp = cJSON_CreateObject();
        cJSON_AddStringToObject(json_connect_esp, "mac", mac_address);
        // 0 Envia
        cJSON_AddNumberToObject(json_connect_esp, "json_type", 0);
#ifdef CONFIG_ENERGIA
        printf("energia\n");
        cJSON_AddStringToObject(json_connect_esp, "type", "energia");
#endif
#ifdef CONFIG_BATERIA
        printf("bateria\n");
        cJSON_AddStringToObject(json_connect_esp, "type", "bateria");
#endif
        mqtt_envia_mensagem(topic_name, cJSON_Print(json_connect_esp));
        cJSON_Delete(json_connect_esp);
      }
      printf("%s\n", topic_name);
      esp_mqtt_client_subscribe(client, topic_name, 0);
      if (first_connection == 0) {
        xSemaphoreGive(conexaoMQTTSemaphore);
      }
      break;
    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
      break;
    case MQTT_EVENT_SUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_UNSUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_PUBLISHED:
      ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_DATA:
      ESP_LOGI(TAG, "MQTT_EVENT_DATA");
      mqtt_response_handler(event->data);
      break;
    case MQTT_EVENT_ERROR:
      ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
      break;
    default:
      ESP_LOGI(TAG, "Other event id:%d", event->event_id);
      break;
  }
  return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
  ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
  mqtt_event_handler_cb(event_data);
}

void mqtt_start(char *topico, char *mac) {
  esp_mqtt_client_config_t mqtt_config = {
      .uri = "mqtt://broker.hivemq.com:1883",
  };

  strcpy(topic_name, topico);
  strcpy(mac_address, mac);

  client = esp_mqtt_client_init(&mqtt_config);
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
  esp_mqtt_client_start(client);
}

void mqtt_envia_mensagem(char *topico, char *mensagem) {
  int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
  ESP_LOGI(TAG, "Mensagem enviada, ID: %d", message_id);
}

void mqtt_response_handler(char *data_from_host) {
  cJSON *raw_json = cJSON_Parse(data_from_host);
  if (raw_json == NULL)
    perror("MQTT");
  else {
    cJSON *json_type = cJSON_GetObjectItemCaseSensitive(raw_json, "json_type");

    if (json_type->valueint == 10 && first_connection == 1) {
      cJSON *device = cJSON_GetObjectItemCaseSensitive(raw_json, "device");
      char *home_location = cJSON_GetObjectItemCaseSensitive(device, "room")->valuestring;
      char *output = cJSON_GetObjectItemCaseSensitive(device, "output")->valuestring;

#ifdef CONFIG_ENERGIA
      char *input = cJSON_GetObjectItemCaseSensitive(device, "input")->valuestring;
      grava_value_nvs("input_name", 0, input, string_type);
#endif

      grava_value_nvs("house_topic", 0, home_location, string_type);
      grava_value_nvs("output_name", 0, output, string_type);

      xSemaphoreGive(conexaoMQTTSemaphore);
    } else if (json_type->valueint == 11) {
      nvs_flash_erase_partition("DadosNVS");
      esp_restart();
    } else if (json_type->valueint == 12) {
      int esp_state = le_valor_nvs("esp_state", "", integer_type);
      esp_state = !esp_state;
      grava_value_nvs("esp_state", esp_state, "", integer_type);
    } else {
      ESP_LOGI(TAG, "Echo do MQTT tratado");
    }
  }
}
