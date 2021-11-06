import mqtt from 'mqtt';

export const options = {
  keepalive: 30,
  protocolId: 'MQTT',
  protocolVersion: 4,
  clean: true,
  reconnectPeriod: 1000,
  connectTimeout: 30 * 1000,
  will: {
    topic: 'WillMsg',
    payload: 'Connection Closed abnormally..!',
    qos: 2,
    retain: false,
  },
  rejectUnauthorized: false,
};

export const mqttConnect = (
  host: any,
  mqttOption: any,
  setClient: any,
  mqtt: any
) => {
  setClient(mqtt.connect(host, mqttOption));
};

export const mqttSub = (subscription: any, client: mqtt.MqttClient) => {
  if (client) {
    const { topic, qos } = subscription;
    console.log(subscription);
    client.subscribe(topic, { qos }, (error) => {
      if (error) {
        console.log('Subscribe to topics error', error);
        return;
      }
      console.log('sub');
    });
  }
};

export const mqttPublish = (context: any, client: mqtt.MqttClient) => {
  if (client) {
    const { topic, qos, payload } = context;
    client.publish(topic, payload, { qos }, (error) => {
      if (error) {
        console.log('Publish error: ', error);
      }
      console.log('pub');
    });
  }
};
