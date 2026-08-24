#include "wifi_manager.h"
#include "secrets.h"
#include <WiFi.h>

void connectWiFi(){
    WiFi.begin(ssid,password);

  Serial.print("Connecting");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  Serial.println("Wi-Fi connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}