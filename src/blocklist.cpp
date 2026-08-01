#include "blocklist.h"
#include "debug.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "hash_table.h"

DomainHashTable blockedDomains;

void loadBlocklist()
{

  File file = LittleFS.open("/blocklist.txt", "r");

  if(!file)
  {
    Serial.println("Couldn't open blocklist.");

    return;
  }

  int count = 0;

  while(file.available())
  {
    String domain = file.readStringUntil('\n');

    domain = normalizeDomain(domain);

    if(domain.length() > 0)
    {
      if(blockedDomains.add(domain))
      {
        count++;
      }
      else
      {
        Serial.println("Hash table full or duplicate");
      }
    }
  }

  file.close();

  Serial.print("Loaded ");
  Serial.print(blockedDomains.size());
  Serial.println(" blocked domains.");
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("Free PSRAM: ");
  Serial.println(ESP.getFreePsram());
}


void saveBlocklist()
{
  File file = LittleFS.open("/blocklist.txt", "w");

  if(!file)
  {
    Serial.println("Couldn't save blocklist.");
    return;
  }

  for(int i = 0; i < blockedDomains.size(); i++)
{
  file.println(blockedDomains.get(i));
}

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

  domain = normalizeDomain(domain);

  if(domain.length() == 0)
  {
    return false;
  }

  if(!blockedDomains.add(domain))
  {
    return false;
  }

  saveBlocklist();

  return true;
}

bool removeBlockedDomain(String domain)
{

  domain.toLowerCase();

  auto result = blockedDomains.remove(domain);

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

  for(int i = 0; i < blockedDomains.size(); i++)
  {
    array.add(blockedDomains.get(i));
  }

  String json;

  serializeJson(doc,json);

  return json;
}

bool checkDomainAndParents(String domain)
{
  while(true)
  {
    if(blockedDomains.contains(domain))
    {
      return true;
    }

    int dot = domain.indexOf('.');

    if(dot == -1)
    {
      break;
    }

    domain = domain.substring(dot + 1);

    //Don't check TLDs
    if(domain.indexOf('.') == -1)
    {
      break;
    }
  }

  return false;
}



bool isBlocked(String domain)
{
  DEBUG_PRINT("Checking: ");
  DEBUG_PRINTLN(domain);

  domain.toLowerCase();

  return checkDomainAndParents(domain);
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
      if(blockedDomains.add(domain))
      {
        result.added ++;
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

int getBlocklistSize()
{
  return blockedDomains.size();
}
void clearBlocklist()
{
  blockedDomains.clear();

  saveBlocklist();
}