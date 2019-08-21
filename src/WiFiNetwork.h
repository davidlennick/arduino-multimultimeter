#ifndef WIFINETWORK_H_
#define WIFINETWORK_H_

#include "Arduino.h"
#include "WiFiNINA.h"

struct WiFiNetwork {
  String ssid;
  byte bssid[6];
  long rssi;
  byte encryption;
};

#endif