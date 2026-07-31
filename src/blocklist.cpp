#include "blocklist.h"
#include "debug.h"
#include <vector>
#include <ArduinoJson.h>
#include <LittleFS.h>

std::vector<String> blockedDomains;

void loadBlocklist()
{
  blockedDomains.clear();

  File file = LittleFS.open("/blocklist.json", "r");

  if(!file)
  {
    Serial.println("Couldn't open blocklist.");

    return;
  }

  JsonDocument doc;

  if(deserializeJson(doc, file))
  {
    Serial.println("Blocklist JSON invalid.");

    file.close();

    return;
  }

  for(JsonVariant domain : doc.as<JsonArray>())
  {
    blockedDomains.push_back(domain.as<String>());
  }

  file.close();

  Serial.print("Loaded ");
  Serial.print(blockedDomains.size());
  Serial.println(" blocked domains.");
}


void saveBlocklist()
{
  File file = LittleFS.open("/blocklist.json", "w");

  if(!file)
  {
    Serial.println("Couldn't save blocklist.");
    return;
  }

  JsonDocument doc;

  JsonArray array = doc.to<JsonArray>();

  for(const String& domain : blockedDomains)
  {
    array.add(domain);
  }

  serializeJsonPretty(doc, file);

  file.close();
  Serial.println("Blocklist saved.");
}

bool addBlockedDomain(String domain)
{
  domain.toLowerCase();

  for(const String& blocked : blockedDomains)
  {
    if(blocked == domain)
    {
      return false;
    }
  }

  blockedDomains.push_back(domain);

  saveBlocklist();

  return true;
}

bool removeBlockedDomain(String domain)
{   
    domain.toLowerCase();

    for(size_t i =0; i < blockedDomains.size(); i++)
    {
      if(blockedDomains[i] == domain)
      {
        blockedDomains.erase(
          blockedDomains.begin() + i
        );

        saveBlocklist();

        return true;
      }
    }

    return false;
}

String createBlocklistJSON()
{
  JsonDocument doc;

  JsonArray array = doc.to<JsonArray>();

  for(const String& domain : blockedDomains)
  {
    array.add(domain);
  }

  String json;

  serializeJson(doc,json);

  return json;
}

bool isBlocked(String domain) {
    DEBUG_PRINT("Checking: ");
    DEBUG_PRINTLN(domain);

    //Match exact domains and subdomains
    //Prevent false positives like notexample.com
    for(size_t i =0; i < blockedDomains.size(); i++) {
      if(domain == blockedDomains[i])
      {
        return true;
      }

      String suffix = "." + blockedDomains[i];

      if(domain.endsWith(suffix)) {
        return true;
      }
    }


    return false;
}