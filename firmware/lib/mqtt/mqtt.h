// co-authored by carlhampuswall

#ifndef mqtt_h
#define mqtt_h

#include <WiFi.h>
#include <PubSubClient.h>

class Mqtt
{
public:
  Mqtt(const char *server, uint16_t port, const char *clientname, const char *username, const char *password);
  static void callback(char *topic, byte *payload, unsigned int length);
  void reconnect();
  void publish(const char *topic, const char *payload);
  void loop();
  PubSubClient getClient();
  String *getLastPayload();

protected:
private:
  const char *_server;
  const char *_clientname;
  const char *_username;
  const char *_password;

  String *lastPayload;

  PubSubClient *_mqttClient;
};

#endif