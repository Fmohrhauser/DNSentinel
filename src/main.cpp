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
#include <LittleFS.h>
#include "settings.h"
#include "time_manager.h"
#include "cache_stats.h"
#include "auth.h"
#include "whitelist.h"
#include "idf_webserver.h"
#include "debug.h"
//variables

//setup
void setup() {
  Serial.begin(115200);
  LittleFS.begin();
  initCache();
  initQueryLog();
  connectWiFi();
  startIDFWebServer();
  startDNSServer();
  initializeSettings();
  initializeAuthentication();
  loadSettings();
  initTime();

  initCacheStats();


  blockedDomains.begin();
  whitelistedDomains.begin();
  
  loadBlocklist();
  loadWhitelist();
  
}

//main program
void loop() {
  handleDNS();

  static unsigned long lastHeapLog = 0;

  if(millis() - lastHeapLog >= 10000)
  {
    lastHeapLog = millis();

    DEBUG_PRINT("Free heap: ");
    DEBUG_PRINTLN(ESP.getFreeHeap());

    DEBUG_PRINT("Minimum free heap: ");
    DEBUG_PRINTLN(ESP.getMinFreeHeap());

    DEBUG_PRINT("Largest free block: ");
    DEBUG_PRINTLN(ESP.getMaxAllocHeap());
  }
}




