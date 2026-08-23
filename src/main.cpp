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


  initializeSettings();
  initializeAuthentication();
  loadSettings();


  initTime();


  startIDFWebServer();
  startDNSServer();
 
  initCacheStats();

  if(!blockedDomains.begin(
    65536,
    3 * 1024 * 1024
  ))
  {
    DEBUG_PRINTLN("BLOCKLIST HASH TABLE INIT FAILED");
  }

  if(!whitelistedDomains.begin(
    8192,
    256 * 1024
  ))
  {
    DEBUG_PRINTLN("WHITELIST HASH TABLE INIT FAILED");
  }
  
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

    DEBUG_PRINT("Free PSRAM: ");
    DEBUG_PRINTLN(ESP.getFreePsram());
  }
}




