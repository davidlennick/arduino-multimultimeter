#ifndef SENSORREADING_H_
#define SENSORREADING_H_

#include "Arduino.h"

struct SensorReading {
  float bus_V;
  float shunt_mV;
  float current_mA;
  float power_mW;
  float load_V;
};

#endif