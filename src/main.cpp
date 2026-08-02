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
#include <LittleFS.h>
#include "api.h"
#include "settings.h"
#include "time_manager.h"


//setup
void setup() {
  Serial.begin(115200);
  if(!LittleFS.begin())
{
    Serial.println("LittleFS mount failed");
}
else
{
    Serial.println("LittleFS mounted");
}
  initCache();
  initQueryLog();
  connectWiFi();
  startDNSServer();
  startDashboard();
  startAPI();
  initTime();
  initializeSettings();
  loadSettings();

  blockedDomains.begin();
  
  loadBlocklist();
  
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




