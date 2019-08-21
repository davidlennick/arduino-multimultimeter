#ifndef MQTTMANAGER_H_
#define MQTTMANAGER_H_

#import "PubSubClient.h"

class MQTTManager {

 public:

  MQTTManager(PubSubClient* client);
  MQTTManager(PubSubClient* client, char* admin_topic);

  String admin_topic();
  void admin_topic(char* topic);

  void Reconnect();

  static void CallbackHandler(char* topic, byte* payload, unsigned int length);
  static void PrintPayload(byte* payload, unsigned int length);

 private:
  PubSubClient* client_;
  char* admin_topic_ = "arduino";
};

#endif