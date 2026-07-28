//libs
#include <arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "dns_parser.h"
#include "blocklist.h"
#include "dns_server.h"
#include "wifi_manager.h"
#include "dns_cache.h"


//setup
void setup() {
  Serial.begin(115200);
  initCache();
  connectWiFi();
  startDNSServer();
}


//main program
void loop() {
  handleDNS();
}




