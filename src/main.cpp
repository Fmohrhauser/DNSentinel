//libs
#include <arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "dns_parser.h"
#include "blocklist.h"
#include "dns_server.h"
#include "wifi_manager.h"
#include "dns_cache.h"
#include "query_log.h"
#include "dashboard.h"


//setup
void setup() {
  Serial.begin(115200);
  initCache();
  initQueryLog();
  connectWiFi();
  startDNSServer();
  startDashboard();
}


//main program
void loop() {
  handleDashboard();
  handleDNS();
  //debug stuff
  if(Serial.available())
  {
    char command = Serial.read();

    if(command == 'l')
    {
      testLatest();
    }
  }
}




