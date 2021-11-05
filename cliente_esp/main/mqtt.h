#ifndef MQTT_H
#define MQTT_H

void mqtt_start(char *topic);

void mqtt_envia_mensagem(char * topico, char * mensagem);

void mqtt_response_handler(char *data_from_host);

#endif