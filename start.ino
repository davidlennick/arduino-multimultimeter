#include <Adafruit_INA219.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <string.h>
#include "src/SensorReading.h"

// const config vars
const int NUM_SENSORS = 4;
const uint8_t SENSOR_ADDRS[] = {0x40, 0x41, 0x44, 0x45};
const byte ETH_MAC[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
const IPAddress BROKER_IP(10, 0, 0, 175);
const int BROKER_PORT = 31883;
const String BASE_TOPIC = "ina219";
const char* ADMIN_TOPIC = "arduinoAdmin";

// global vars
Adafruit_INA219* sensors[NUM_SENSORS];
SensorReading reading;
EthernetClient eth_client;
PubSubClient mqtt_client(eth_client);

// json vars
const size_t capacity = JSON_OBJECT_SIZE(6);
DynamicJsonDocument doc(capacity);

void setup(void) {
  // init and wait for serial connection
  Serial.begin(115200);
  // while (!Serial) {
  // }

  Serial.println("========================================");
  Serial.println("Starting setup...");
  Serial.println("========================================");
  Serial.println();

  InitSensors();
  InitEth();
  InitMQTT();

  // delay(100);

  Serial.println();
  Serial.println("========================================");
  Serial.println("Done setup");
  Serial.println("========================================");
}

void loop(void) {
  String full_topic = "";
  char json_buffer[100];
  size_t n;

  CheckConn();

  // https://www.arduino.cc/reference/en/language/variables/utilities/sizeof/
  for (byte i = 0; i < (sizeof(sensors) / sizeof(sensors[0])); i++) {
    full_topic = BASE_TOPIC + "/";
    reading = GetSensorReading(sensors[i]);

    // create the topic to publish to
    full_topic += String(SENSOR_ADDRS[i], HEX);

    // convert sensor readings to json and print to console
    SensorReadingToJson(String(SENSOR_ADDRS[i], HEX), reading, doc);

    // store the json doc in a buffer, print, and publish
    n = serializeJson(doc, json_buffer);
    Serial.print("[");
    Serial.print(full_topic);
    Serial.print("] ");
    Serial.println(json_buffer);
    mqtt_client.publish((char*)full_topic.c_str(), json_buffer);
    delay(50);
  }

  //delay(200);

  // Serial.print("Free RAM: ");
  // Serial.println(FreeRAM());
}

void InitSensors() {  
  
  // start the INA219 sensors by address
  int num_sensors = sizeof(SENSOR_ADDRS) / sizeof(uint8_t);

  for (int i = 0; i < num_sensors; i++) {
    sensors[i] = new Adafruit_INA219(SENSOR_ADDRS[i]);
    sensors[i]->begin();
    delay(100);
  }
  
}

void InitEth() {
  Serial.println("Starting eth setup...");
  // Ethernet.begin(ETH_MAC);
  
  if (Ethernet.begin(ETH_MAC) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while (true) {
    }
  }

  Serial.println(Ethernet.localIP());
  Serial.println("Done eth setup.");
}

void InitMQTT() {  
  // init mqtt client
  mqtt_client.setServer(BROKER_IP, BROKER_PORT);
  ReconnectMQTT();
  
}

void CheckConn() {
  if (!mqtt_client.connected()) {
    Serial.println("Lost MQTT connection, attempting reconnect...");
    ReconnectMQTT();
  }
}

SensorReading GetSensorReading(Adafruit_INA219* sensor) {
  SensorReading r;
  // delay(100);
  r.shunt_mV = sensor->getShuntVoltage_mV();
  // delay(100);
  r.bus_V = sensor->getBusVoltage_V();
  // delay(100);
  r.current_mA = sensor->getCurrent_mA();
  // delay(100);
  r.power_mW = sensor->getPower_mW();
  r.load_V = r.bus_V + (r.shunt_mV / 1000);
  return r;
}

void PrintSensorReading(SensorReading reading) {
  Serial.print("Bus Voltage:   ");
  Serial.print(reading.bus_V);
  Serial.println(" V");
  Serial.print("Shunt Voltage: ");
  Serial.print(reading.shunt_mV);
  Serial.println(" mV");
  Serial.print("Load Voltage:  ");
  Serial.print(reading.load_V);
  Serial.println(" V");
  Serial.print("Current:       ");
  Serial.print(reading.current_mA);
  Serial.println(" mA");
  Serial.print("Power:         ");
  Serial.print(reading.power_mW);
  Serial.println(" mW");
}

void SensorReadingToJson(String addr, SensorReading reading, DynamicJsonDocument& doc) {
  doc["addr"] = addr.toInt();
  doc["bus_V"] = reading.bus_V;
  doc["shunt_mV"] = reading.shunt_mV;
  doc["load_V"] = reading.load_V;
  doc["current_mA"] = reading.current_mA;
  doc["power_mW"] = reading.power_mW;
}

void ReconnectMQTT() {
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqtt_client.connect("arduinoClient")) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      mqtt_client.publish(ADMIN_TOPIC, "hello world");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// https://forum.arduino.cc/index.php?topic=215334.msg1576106#msg1576106
int FreeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}