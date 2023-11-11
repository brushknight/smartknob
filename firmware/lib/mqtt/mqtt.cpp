#include "mqtt.h"
// char* _server;
// char* _clientname;
// char* _username;
// char* _password;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

static const char *TAG = "MQTT";

Mqtt::Mqtt(const char *server, uint16_t port, const char *clientname, const char *username, const char *password)
{
  delay(10);
  _server = server;
  _clientname = clientname;
  _username = username;
  _password = password;

  mqttClient.setServer(server, port);
  mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                         { this->callback(topic, payload, length); });

  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();
}

void Mqtt::reconnect()
{
  while (!mqttClient.connected())
  {
    ESP_LOGD(TAG, "Attempting connection %s %s %s", _clientname, _username, _password);

    if (mqttClient.connect(_clientname, _username, _password))
    { //
      // Serial.println("Connected");
      ESP_LOGI(TAG, "Connected");

      mqttClient.subscribe("smartknob/output");
    }
    else
    {
      ESP_LOGE(TAG, "Failed %d", mqttClient.state());

      delay(5000);
    }
  }
}

void Mqtt::callback(char *topic, byte *payload, unsigned int length)
{
  // Serial.print("Message received on topic: ");
  // Serial.print(topic);
  // Serial.print(". Message: ");
  String payloadTemp;

  for (int i = 0; i < length; i++)
  {
    // Serial.print((char)payload[i]);
    payloadTemp += (char)payload[i];
  }
  // Serial.println();
}

void Mqtt::publish(const char *topic, const char *payload)
{

  mqttClient.publish(topic, payload);
}

void Mqtt::loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();
}