#include "WiFiManager.h"

// https://www.arduino.cc/en/Reference/WiFiNINA

// wrapper/facade class to shorten some of the wifi library stuff

WiFiManager::WiFiManager(WiFiClient* client) { this->client_ = client; }

WiFiNetwork WiFiManager::current_network() { return this->network_; }

void WiFiManager::Init() {
  if (this->client_->status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true) {
    }
  }

  if (WiFi.firmwareVersion() != WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade firmware");
    Serial.println(WiFi.firmwareVersion());
    Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
  }
}

bool WiFiManager::ConnectWiFi() {
  // attempt to connect to WiFi network:
  byte counter = 0;
  byte attempt_limit = 5;

  while (WiFi.status() != WL_CONNECTED && counter < attempt_limit) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SECRET_SSID);

    // Connect to WPA/WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);

    delay(5000);
    counter++;
  }

  if (this->client_->status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi!");
    return true;
  }

  return false;
}

void WiFiManager::UpdateCurrentNetwork() {
  this->network_.ssid = WiFi.SSID();
  this->network_.rssi = WiFi.RSSI();
  WiFi.BSSID(this->network_.bssid);
  this->network_.encryption = WiFi.encryptionType();
}

void WiFiManager::PrintWiFiClientData() {
  // print your board's IP address:
  byte mac[6];

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("MAC address: ");
  WiFiManager::PrintMACAddress(WiFi.macAddress(mac));
}

void WiFiManager::PrintCurrentNetwork() {
  this->UpdateCurrentNetwork();
  WiFiManager::PrintNetwork(this->network_);
}

// static methods

void WiFiManager::PrintMACAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void WiFiManager::PrintNetwork(WiFiNetwork network) {
  Serial.print("SSID: ");
  Serial.println(network.ssid);

  Serial.print("BSSID: ");
  WiFiManager::PrintMACAddress(network.bssid);

  Serial.print("Signal Strength (RSSI):");
  Serial.print(network.rssi);
  Serial.println(" dBm");

  Serial.print("Encryption Type:");
  Serial.println(network.encryption, HEX);
}