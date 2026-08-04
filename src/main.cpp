//libs
#include <arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include "dns_parser.h"
#include "blocklist.h"
#include "dns_server.h"
#include "wifi_manager.h"
#include "dns_cache.h"
#include "query_log.h"
#include "dashboard.h"
#include <LittleFS.h>
#include "api.h"
#include "settings.h"
#include "time_manager.h"
#include "cache_stats.h"
#include "auth.h"

extern WebServer server;
//setup
void setup() {
  Serial.begin(115200);
  LittleFS.begin();
  initCache();
  initQueryLog();
  connectWiFi();
  startDNSServer();
  loadConfig();
  initializeSettings();
  loadSettings();
    const char* headerKeys[] = {"Authorization"};

  server.collectHeaders(
      headerKeys,
      1
  );
  startDashboard();
  startAPI();
  initTime();

  initCacheStats();


  blockedDomains.begin();
  
  loadBlocklist();

Serial.println(deviceConfig.deviceName);
Serial.println(deviceConfig.upstreamDNS);
Serial.println(deviceConfig.UPSTREAM_TIMEOUT);
  
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




