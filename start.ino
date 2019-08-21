/*
  Done:
    - read INA219 sensor values
    - connect to WiFi network

  In progress:
  - connect to MQTT server
  - serialize readings as JSON
  - publish readings to MQTT broker
 */

#include <string.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiNINA.h>
#include "src/MQTTManager.h"
#include "src/SensorReader.h"
#include "src/SensorReading.h"
#include "src/WiFiManager.h"

SensorReader* readers[4];

WiFiClient* wifi_client;
WiFiManager* wifi_manager;

IPAddress broker_ip(10, 0, 0, 175);
const int broker_port = 31883;
PubSubClient* mqtt_client;
MQTTManager* mqtt_manager;

const size_t capacity = JSON_OBJECT_SIZE(6);
DynamicJsonDocument doc(capacity);

char json_buffer[512];
SensorReading reading;
int delay_val = 200;

void setup(void) {
  // init and wait for serial connection
  Serial.begin(19200);
  while (!Serial) {
  }

  Serial.println("Starting setup");
  InitSensorReaders();
  InitWiFi();
  InitMQTT();
  Serial.println("Done setup...");
}

void loop(void) {
  char* base_topic = "outTopic/";
  char* addr;
  char* full_topic;

  memset(json_buffer, 0, sizeof(json_buffer));

  CheckConn();

  // https://www.arduino.cc/reference/en/language/variables/utilities/sizeof/

  for (byte i = 0; i < (sizeof(readers) / sizeof(readers[0])); i++) {
    reading = readers[i]->GetUpdatedReading();
    addr = StringToChar(readers[i]->GetAddrAsString());

    // create the topic to publish to
    strcat(full_topic, base_topic);
    strcat(full_topic, addr);

    // String full_topic = String(base_topic + addr);
    // int str_len = full_topic.length() + 1;
    // char* buffer[str_len];
    // full_topic.toCharArray(buffer, str_len);

    // // convert sensor readings to json and print to console
    // SensorReadingToJson(addr, reading, doc);
    // PrintReadingJson(doc, full_topic);

    // // store the json doc in a buffer
    // serializeJson(doc, json_buffer);
    // mqtt_client->publish(full_topic, json_buffer);

    // delay(delay_val);
  }
  Serial.print("Free RAM: ");
  Serial.println(FreeRAM());
  delay(1000);
  // delete base_topic;
  // delete addr;
  // delete full_topic;
}

void InitSensorReaders() {
  // start the INA219 sensors by address
  // TODO: add logic for specifying number of sensor readers/addresses
  readers[0] = new SensorReader(0x40);
  readers[1] = new SensorReader(0x41);
  readers[2] = new SensorReader(0x44);
  readers[3] = new SensorReader(0x45);
}

void InitWiFi() {
  // init WiFi
  wifi_client = new WiFiClient();
  wifi_manager = new WiFiManager(wifi_client);

  wifi_manager->Init();
  wifi_manager->ConnectWiFi();

  Serial.println("\n========================================");
  wifi_manager->PrintWiFiClientData();
  wifi_manager->UpdateCurrentNetwork();
  wifi_manager->PrintCurrentNetwork();
  Serial.println("========================================\n");
}

void InitMQTT() {
  // init mqtt client
  mqtt_client = new PubSubClient(*wifi_client);
  mqtt_manager = new MQTTManager(mqtt_client, "arduinoMonitor");

  mqtt_client->setServer(broker_ip, broker_port);
  mqtt_client->setCallback(MQTTManager::CallbackHandler);
  mqtt_manager->Reconnect();
}

void CheckConn() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Lost Wifi connection. Status: ");
    Serial.println(WiFi.status());
    Serial.println("Attempting reconnect...");
    wifi_manager->ConnectWiFi();
    wifi_manager->PrintWiFiClientData();
    wifi_manager->UpdateCurrentNetwork();
    wifi_manager->PrintCurrentNetwork();
    mqtt_manager->Reconnect();
  }

  if (!mqtt_client->connected()) {
    Serial.println("Lost MQTT connection, attempting reconnect...");
    mqtt_manager->Reconnect();
  }
}

void BuildTopic(String base_topic, String addr, char*& buf) {}

void SensorReadingToJson(String addr, SensorReading reading, DynamicJsonDocument& doc) {
  doc["addr"] = addr.toInt();
  doc["bus_V"] = reading.bus_V;
  doc["shunt_mV"] = reading.shunt_mV;
  doc["load_V"] = reading.load_V;
  doc["current_mA"] = reading.current_mA;
  doc["power_mW"] = reading.power_mW;
}

void PrintReadingJson(DynamicJsonDocument doc, char* topic) {
  Serial.println(topic);
  serializeJson(doc, Serial);
  Serial.println();
}

// https://forum.arduino.cc/index.php?topic=215334.msg1576106#msg1576106
int FreeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

// https://coderwall.com/p/zfmwsg/arduino-string-to-char
char* StringToChar(String command) {
  if (command.length() != 0) {
    char* p = const_cast<char*>(command.c_str());
    return p;
  }
}