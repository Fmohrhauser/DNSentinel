//libs
#include <arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "dns_parser.h"
#include "blocklist.h"
#include "dns_server.h"
#include "wifi_manager.h"


//setup
void setup() {
  Serial.begin(115200);
  connectWiFi();
  startDNSServer();
}


//main program
void loop() {
  handleDNS();
}




