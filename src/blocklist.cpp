#include "blocklist.h"
#include "debug.h"
#include <set>
#include <ArduinoJson.h>
#include <LittleFS.h>

std::set<String> blockedDomains;

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
    blockedDomains.insert(domain.as<String>());
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

String normalizeDomain(String line)
{
  line.trim();

  if(line.length() ==0)
    return "";

  //skip comments
  if(line.startsWith("#"))
    return "";

  if(line.startsWith("!"))
    return "";

  //Remove AdBlock syntax
  if(line.startsWith("||"))
  {
    line.remove(0,2);
  }

  if(line.endsWith("^"))
  {
    line.remove(line.length() - 1);
  }

  //Remove hosts file IPs
  if(line.startsWith("0.0.0.0 "))
  {
    line.remove(0,8);
  }

  if(line.startsWith("127.0.0.1 "))
  {
    line.remove(0,10);
  }

  line.trim();
  line.toLowerCase();

  if(line == "localhost")
    return "";

  for(char c : line)
  {
    if(
      !(isalnum(c) ||
      c == '.' ||
      c == '-')
    )
    {
      return "";
    }
  }

  return line;
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
  domain = normalizeDomain(domain);
  blockedDomains.insert(domain);

  saveBlocklist();

  return true;
}

bool removeBlockedDomain(String domain)
{

  domain.toLowerCase();

  auto result = blockedDomains.erase(domain);

  if(result)
  {
    saveBlocklist();
    return true;
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

  for(const String& blocked : blockedDomains)
  {

    if(domain == blocked)
    {
      return true;
    }


    String suffix = "." + blocked;

    if(domain.endsWith(suffix))
    {
      return true;
    }
  }


  return false;

}


ImportResult importBlocklist(String data)
{

  ImportResult result;

  int start = 0;

  while(start < data.length())
  {
    int end = data.indexOf('\n', start);

    if(end == -1)
    {
      end = data.length();
    }

    String domain = data.substring(start, end);

    start = end + 1;

    domain = normalizeDomain(domain);

    if(domain.length() == 0)
    {
      result.ignored++;
    }
    else
    {
      size_t oldSize = blockedDomains.size();

      blockedDomains.insert(domain);

      if(blockedDomains.size() > oldSize)
      {
        result.added++;
      }
      else
      {
          result.duplicates++;        
      }
    }
  }

  saveBlocklist();

  return result;
}