#import "MQTTManager.h"

// wrapper/facade class to shorten some of the mqtt calls

MQTTManager::MQTTManager(PubSubClient* client) { this->client_ = client; }

MQTTManager::MQTTManager(PubSubClient* client, char* admin_topic) {
  this->client_ = client;
  this->admin_topic(admin_topic);
}

String MQTTManager::admin_topic() { return this->admin_topic_; }
void MQTTManager::admin_topic(char* topic) { this->admin_topic_ = topic; }

void MQTTManager::Reconnect() {
  
  // Loop until we're reconnected
  while (!this->client_->connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (this->client_->connect("arduinoClient")) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      this->client_->publish(this->admin_topic_, "hello world");

      // ... and resubscribe
      this->client_->subscribe("inTopic");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(this->client_->state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


// static
void MQTTManager::PrintPayload(byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void MQTTManager::CallbackHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if (!strcmp(topic, "inTopic")) {
    MQTTManager::PrintPayload(payload, length);
  }
  else {
    Serial.print("unknown topic, can't handle: ");
    Serial.println(topic);
  }
}
