#include "SensorReader.h"

// public

SensorReader::SensorReader(uint8_t addr) {
  this->addr_ = addr;
  this->sensor_ = Adafruit_INA219(this->addr_);
  this->sensor_.begin();
  this->reading_ = SensorReading();
}

SensorReader::SensorReader(uint8_t addr, Adafruit_INA219 sensor) {
  this->addr_ = addr;
  this->sensor_ = sensor;
  this->sensor_.begin();
  this->reading_ = SensorReading();
}

Adafruit_INA219 SensorReader::sensor() { return this->sensor_; }

SensorReading SensorReader::reading() { return this->reading_; }

uint8_t SensorReader::addr() { return this->addr_; }

void SensorReader::UpdateReading() {
  this->reading_.shunt_mV = this->sensor_.getShuntVoltage_mV();
  this->reading_.bus_V = this->sensor_.getBusVoltage_V();
  this->reading_.current_mA = this->sensor_.getCurrent_mA();
  this->reading_.power_mW = this->sensor_.getPower_mW();
  this->reading_.load_V = this->reading_.bus_V + (this->reading_.shunt_mV / 1000);
}

SensorReading SensorReader::GetUpdatedReading() {
  this->UpdateReading();
  return this->reading();
}

String SensorReader::GetAddrAsString() {
  return String(this->addr(), HEX);
}

void SensorReader::PrintUpdatedReading() {
  this->UpdateReading();
  SensorReader::PrintSensorReading(this->reading());
}

// static
void SensorReader::PrintSensorReading(SensorReading reading) {
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